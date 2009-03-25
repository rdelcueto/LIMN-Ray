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

#include <list>
#include <limits.h>
#include <math.h>
#include "scene.h"

#include "primitives.cpp"
#include "materials.cpp"
#include "lights.cpp"

Scene::Scene() {
  plane_w = 640;
  plane_h = 480;
  planePos[0] = planePos[1] = planePos[2] = 0.0;
  focalPointPos[0] = focalPointPos[1] = 0.0; focalPointPos[2] = -1.7;
  lookAt[0] = lookAt[1] = 0.0; lookAt[2] = 1.0;
  recurLimit = 1;
  samples = 1;

  sceneLights.push_back(new Omnidirectional(0, 5, 25, 1, 0, 0, 1, 4));
  sceneLights.push_back(new Omnidirectional(-7.07, -7.07, 25, 0, 1, 0, 1, 4));
  sceneLights.push_back(new Omnidirectional(7.07, -7.07, 25, 0, 0, 1, 1, 4));

  sceneMaterials.push_back(new Material());
  Material *m = NULL;
  MaterialList::iterator mi = sceneMaterials.begin();
  m = *mi;

  sceneObjects.push_back(new Sphere(0, -5, 50, 5, m));
  sceneObjects.push_back(new Sphere(-7.07, 7.07, 75, 5, m));
  sceneObjects.push_back(new Sphere(7.07, 7.07, 100, 5, m));
  //sceneObjects.push_back(new Sphere(0, 0, 50, 4, m));
}

void Scene::intersect(Ray *r) {
  register double minT = r->t_intersect;
  double currT;
  Primitive *p = NULL;
  PrimitiveList::iterator i;
  for(i = sceneObjects.begin(); i != sceneObjects.end(); ++i) {
    p = *i;
    currT = p->intersect(r);
    if(currT < minT && currT > 0) {
      minT = currT;
      r->setIntersection(minT, p);
    }
  }
}

void Scene::shader(Ray *r, double *color) {
  Light *l = NULL;
  LightList::iterator i;

  double phongD, phongS, pDistance, lDistance, lightI;

  // Intersection Point
  double *intPoint = new double[3];
  r->getPosAtInt(intPoint);

  // Surface Normal @ Intersection Point
  Primitive *p = r->p_intersect;
  double *normP = new double[3];
  p->normalAtP(intPoint, normP);

  // View Unit Vector from Intersection Point
  double *v = new double[3];
  blas3dSubstractXY(r->pos, intPoint, v); // View = Ray Pos - Intersection
  pDistance = sqrt(blasFast3dDot(v, v))/2;
  blasFast3DNormalize(v);

  // Light Unit Vector from Intersection Point
  double *lVec = new double[3];
  // h = (lVec + View)/2
  double *h = new double[3];
  // |d| = (hdotn)n-h Specular Phong Reformulation
  double *d = new double[3];

  Material *m = p->material;
  double pixelColor[3] = {m->color[0]*m->color[3],
                          m->color[1]*m->color[3],
                          m->color[2]*m->color[3]};

  for(i = sceneLights.begin(); i != sceneLights.end(); ++i) {
    l = *i;

    // Phong
    blas3dSubstractXY(l->pos, intPoint, lVec); // Light Pos - Intersection
    lDistance = pDistance + sqrt(blasFast3dDot(lVec, lVec))/2;
    blasFast3DNormalize(lVec);

    lightI = l->intensity/(lDistance*lDistance*.001/l->damping);
    // Diffuse
    phongD = blasFast3dDot(lVec, normP) * m->diffuse * lightI;

    // Specular
    blas3dAddXY(lVec, v, h);
    blasDscal(3, 0.5, h, 1);
    blasDcopy(3, normP, 1, d, 1);
    blasDscal(3, blasFast3dDot(h,d), d, 1);
    blas3dSubstractXY(d, h, d);
    phongS = (m->specular_Hardness/2)*blasFast3dDot(d, d);
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

  color[0] += pixelColor[0] * m->ambient;
  color[1] += pixelColor[1] * m->ambient;
  color[2] += pixelColor[2] * m->ambient;

  delete [] intPoint;
  delete [] normP;
  delete [] lVec;
  delete [] d;
  delete [] v;
  intPoint = normP = lVec = d = v = NULL;
}
