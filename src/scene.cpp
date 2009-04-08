/*
 * File: scene.h
 * Author: Rodrigo González del Cueto
 */

/*
 * This file is part of Limn-Ray.
 * Limn-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Limn-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Limn-Ray.  If not, see <http://www.gnu.org/licenses/>.
 */

#define ENABLE_SHADOWS 1
#define INTERSECT_EPSILON 0.0001
#define LIGHT_SCALE 10000

#include <list>
#include <limits.h>
#include <math.h>
#include "scene.h"

#include "primitives.cpp"
#include "materials.cpp"
#include "lights.cpp"

Scene::Scene() {
  plane_w = 1920;
  plane_h = 1080;
  planePos[0] = planePos[1] = planePos[2] = 0.0;
  focalPointPos[0] = focalPointPos[1] = 0.0; focalPointPos[2] = -3.0;
  lookAt[0] = lookAt[1] = 0.0; lookAt[2] = 1.0;
  recurLimit = 8;
  samples = 1;

  sceneMaterials.push_back(new Material(
      1.0, 0.0, 0.0, 1.0,  0.0, 1, 1, 2, 1.0, 1.0));
  sceneMaterials.push_back(new Material(
      0.0, 0.0, 1.0, 1.0,  0.0, 1, 1, 2, 1.0, 1.0));
  sceneMaterials.push_back(new Material(
      0.0, 1.0, 0.0, 1.0,  0.0, 1, 1, 2, 1.0, 1.0));
  sceneMaterials.push_back(new Material(
      1.0, 1.0, 1.0, 0.15,  0.0, 0.1, 1, 16, 0, 1.0));
  sceneMaterials.push_back(new Material(
      0.2, 0.2, 0.2, 1.0,  0.1, 1, 1, 1, 0, 1.0));

  MaterialList::iterator mi = sceneMaterials.begin();

  sceneLights.push_back(new Omnidirectional(-14.14, 100, 200, 1, 0, 0, 2, 1));
  sceneLights.push_back(new Omnidirectional(14.14, 100, 200, 0, 0, 1, 2, 1));
  sceneLights.push_back(new Omnidirectional(0, 100, 224.14, 0, 1, 0, 2, 1));

  sceneLights.push_back(new Omnidirectional(-141.14, 100, 200, 1, 1, 1, 5, 1));
  sceneLights.push_back(new Omnidirectional(141.14, 100, 200, 1, 1, 1, 5, 1));
  sceneLights.push_back(new Omnidirectional(0, 100, 324.14, 1, 1, 1, 5, 1));

  sceneObjects.push_back(new Sphere(-14.14, 0, 200, 8, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(14.14, 0, 200, 8, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 0, 224.14, 8, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 0, 212.07, 5, *mi));
  mi++;
  sceneObjects.push_back(new Plane(0, -10, 0, 0, 1, 0, *mi));

}

double Scene::intersect(Ray *r) {
  double minT = std::numeric_limits<double>::infinity();
  double currT;
  Primitive *p = NULL;
  Primitive *pMin = NULL;
  PrimitiveList::iterator i;
  for(i = sceneObjects.begin(); i != sceneObjects.end(); ++i) {
    p = *i;
    currT = p->intersect(r);
    if(currT < minT && currT > INTERSECT_EPSILON) {
      minT = currT;
      pMin = p;
    }
  }
  if(minT < r->intersect_t) {
    r->intersect_t = minT;
    r->intersect_material = pMin->material;
    r->getPosAtT(r->intersect_t, r->intersect_point);
    pMin->normalAtP(r->intersect_point, r->intersect_normal);
    return pMin->material->color[3];
  }
  else {
    r->intersect_t = std::numeric_limits<double>::infinity();
    return 0.0;
  }
}

int Scene::shader(Ray *r) {
  Light *l = NULL;
  LightList::iterator i;
  int secondaryRays = 0;

  if(r->weight > 0) {
    // Shading Pixel
    double *color = r->ray_pixel;
    // Intersection Point
    double *intPoint = r->intersect_point;
    // Surface Normal @ Intersection Point
    double *normP = r->intersect_normal;
    // View Unit Vector from Intersection Point
    double v[3];

    double phongD, phongS, pDistance, lDistance, lightI;
    double pixelColor[3];

    Material *m = r->intersect_material;

    pixelColor[0] = r->weight[0]*m->color[0];
    pixelColor[1] = r->weight[1]*m->color[1];
    pixelColor[2] = r->weight[2]*m->color[2];

    color[0] += pixelColor[0] * m->ambient;
    color[1] += pixelColor[1] * m->ambient;
    color[2] += pixelColor[2] * m->ambient;

    r->type = 0;
    if(m->color[3] != 1.0) {
      r->type++;
      secondaryRays++;
    }
    if(m->reflection != 0.0) {
      r->type+=2;
      secondaryRays++;
    }

    blas3dSubstractXY(r->position, intPoint, v); // View = Ray Pos - Intersection
    pDistance = sqrt(blas3dDot(v, v));
    r->sum_ts += pDistance;
    blas3DNormalize(v);
    // Light Unit Vector from Intersection Point
    double lVec[3];
    // h = (lVec + View)/2
    double h[3];
    // |d| = (hdotn)n-h Specular Phong Reformulation
    double d[3];

    for(i = sceneLights.begin(); i != sceneLights.end(); ++i) {
      l = *i;

      blas3dSubstractXY(l->pos, intPoint, lVec); // Light Pos - Intersection
      lDistance = sqrt(blas3dDot(lVec, lVec));
      blas3DNormalize(lVec);

      Ray *shadowRay;
      double shadowL = 1.0;

      if(ENABLE_SHADOWS) {
        shadowRay = new Ray(NULL, intPoint, lVec);
        shadowRay->move2IntNrm();

        do {
          shadowL -= intersect(shadowRay);
          blas3Dcopy(shadowRay->intersect_point, shadowRay->position);
          shadowRay->move2Dir();
          }
        while(shadowRay->intersect_t != std::numeric_limits<double>::infinity()
            && shadowL > 0);

        if(shadowL < 0) shadowL = 0;
      }

      if((r->weight[0] + r->weight[1] + r->weight[2]) == 0 &&
          !ENABLE_SHADOWS || shadowL > 0 ){

        lDistance += r->sum_ts;
        lightI = l->intensity*shadowL*LIGHT_SCALE/(lDistance*lDistance*l->damping);
        // Diffuse
        phongD = blas3dDot(lVec, normP) * m->diffuse * lightI;

        // Specular
        blas3dAddXY(lVec, v, h); // LightPos + EyePos = Halfway = h
        blasDscal(3, 0.5, h, 1); // h = Halfway / 2
        blasDcopy(3, normP, 1, d, 1);
        blasDscal(3, blas3dDot(h,d), d, 1); // (NdotNormAtP)NormAtP
        blas3dSubstractXY(d, h, d); //
        phongS = (m->specular_Hardness/2)*blas3dDot(d, d);

        if (phongS > 1) phongS = 0;
        else {
          phongS = (1-phongS)/2;
          phongS *= phongS;
        }

        if(phongD > 0) {
          color[0] += l->color[0] * phongD * pixelColor[0] * m->color[3];
          color[1] += l->color[1] * phongD * pixelColor[1] * m->color[3];
          color[2] += l->color[2] * phongD * pixelColor[2] * m->color[3];
        }
        if(phongS > 0) {
          color[0] += l->color[0] * phongS * pixelColor[0];
          color[1] += l->color[1] * phongS * pixelColor[1];
          color[2] += l->color[2] * phongS * pixelColor[2];
        }
      }
      if(ENABLE_SHADOWS) delete shadowRay;
    }
  }
  return secondaryRays;
}
