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
#define LIGHT_SCALE 100000

#include <list>
#include <limits.h>
#include <math.h>
#include "scene.h"

#include "primitives.cpp"
#include "materials.cpp"
#include "lights.cpp"

Scene::Scene() {
  plane_w = 1024;
  plane_h = 768;
  planePos[0] = planePos[1] = planePos[2] = 0.0;
  focalPointPos[0] = focalPointPos[1] = 0.0; focalPointPos[2] = -2.8;
  lookAt[0] = lookAt[1] = 0.0; lookAt[2] = 1.0;
  recurLimit = 4;
  samples = 1;

  sceneMaterials.push_back(new Material(1, 0, 0,  0.66, 0.0, 1, 1, 1, 0.25, 0.0));
  sceneMaterials.push_back(new Material(0, 0, 1,  0.66, 0.0, 1, 1, 1, 0.25, 0.0));
  sceneMaterials.push_back(new Material(0, 1, 0,  0.66, 0.0, 1, 1, 1, 0.25, 0.0));
  sceneMaterials.push_back(new Material(0.33, 0.33, 0.33,  1.0, 0.33, 1, 1, 1, 0.5, 0.0));

  MaterialList::iterator mi = sceneMaterials.begin();

  sceneLights.push_back(new Omnidirectional(0, 0, 100, 1, 1, 1, 1, 5));

  sceneObjects.push_back(new Sphere(-14.14, 0, 200, 10, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(14.14, 0, 200, 10, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 0, 224.14, 10, *mi));
  mi++;
  sceneObjects.push_back(new Plane(0, -10, 0, 0, 1, 0, *mi));

}

void Scene::intersect(Ray *r) {
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
  if(minT < r->t_intersect) {
    r->t_intersect = minT;
    r->p_intersect = pMin;
    r->getPosAtT(r->t_intersect, r->int_p);
    pMin->normalAtP(r->int_p, r->int_n);
  }
}

int Scene::shader(Ray *r) {
  Light *l = NULL;
  LightList::iterator i;

  double phongD, phongS, pDistance, lDistance, lightI;

  Primitive *p = r->p_intersect;
  Material *m = p->material;
  double pixelColor[3] = {m->color[0]*m->color[3]*r->colorWeight,
                          m->color[1]*m->color[3]*r->colorWeight,
                          m->color[2]*m->color[3]*r->colorWeight};
  r->colorWeight = m->color[3];

  // Shading Pixel
  double *color = r->pixel;
  // Intersection Point
  double *intPoint = r->int_p;
  // Surface Normal @ Intersection Point
  double *normP = r->int_n;
  // View Unit Vector from Intersection Point
  double v[3];
  blas3dSubstractXY(r->pos, intPoint, v); // View = Ray Pos - Intersection
  pDistance = sqrt(blas3dDot(v, v));
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
    if(ENABLE_SHADOWS) {
      shadowRay = new Ray(NULL, intPoint, lVec);
      shadowRay->move2IntNrm();
      intersect(shadowRay);
    }

    if(!ENABLE_SHADOWS || shadowRay->t_intersect > lDistance ){
      lDistance += pDistance;

      lightI = l->intensity*LIGHT_SCALE/(lDistance*lDistance*l->damping);
      // Diffuse
      phongD = blas3dDot(lVec, normP) * m->diffuse * lightI;

      // Specular
      blas3dAddXY(lVec, v, h); // LightPos + EyePos = Halfway = h
      //std::cout << "HW: " << h[0] << ',' << h[1] << ',' << h[2] << std::endl;
      //std::cout << "Norm: " << normP[0] << ',' << normP[1] << ',' << normP[2] << std::endl;
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
        color[0] += l->color[0] * phongD * pixelColor[0];
        color[1] += l->color[1] * phongD * pixelColor[1];
        color[2] += l->color[2] * phongD * pixelColor[2];
      }
      if(phongS > 0) {
        color[0] += l->color[0] * phongS * pixelColor[0];
        color[1] += l->color[1] * phongS * pixelColor[1];
        color[2] += l->color[2] * phongS * pixelColor[2];
      }
    }
    if(ENABLE_SHADOWS) delete shadowRay;
  }
  color[0] += pixelColor[0] * m->ambient;
  color[1] += pixelColor[1] * m->ambient;
  color[2] += pixelColor[2] * m->ambient;

  int secondaryRays = 0;
  r->type = 0;
  if(m->color[3] != 1.0) {
    r->type++;
    secondaryRays++;
  }
  if(m->reflection != 0.0) {
    r->type+=2;
    secondaryRays++;
  }
  return secondaryRays;
}
