/*
 * File: primitives.cpp
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

#include <iostream>
#include "primitives.h"
#include "blas.h"

class Plane : public Primitive {
public:
  double planeN[3];

  Plane() {
    pos[0] = 0.0; pos[1] = 0.0; pos[2] = 0.0;
    planeN[0] = 0.0; planeN[1] = 1.0; planeN[2] = 0.0;
    material = new Material();
  }

  Plane(double posx, double posy, double posz,
      double normalx, double normaly, double normalz) {
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    planeN[0] = normalx; planeN[1] = normaly; planeN[2] = normalz;
    blas3DNormalize(planeN);
    material = new Material();
  }

  Plane(double posx, double posy, double posz,
      double normalx, double normaly, double normalz,
      Material *m) {
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    planeN[0] = normalx; planeN[1] = normaly; planeN[2] = normalz;
    blas3DNormalize(planeN);
    material = m;
  }

  void normalAtP(const double *p, double *n) {
    n[0] = planeN[0]; n[1] = planeN[1]; n[2] = planeN[2];
  }

  double intersect(Ray *r) {
    double nDotDir = blas3dDot(planeN, r->dir);
    if (nDotDir != 0) {
      double line[3];
      blas3dSubstractXY(r->pos, pos, line);
      double nDotLine = -blas3dDot(planeN, line);
      if(nDotLine != 0)
        return nDotLine/nDotDir;
    }
    return std::numeric_limits<double>::infinity();
  }
};

class Sphere : public Primitive {
public:
  double radius;
  double sqrRadius;

  Sphere() {
    radius = 1.0;
    sqrRadius = 1.0;
    pos[0] = 0.0; pos[1] = 0.0; pos[2] = 100.0;
    bbox[0] = 0.5; bbox[1] = 0.5; bbox[2] = 0.5;
    material = new Material();
  }

  Sphere(double x, double y, double z, double rad) {
    radius = rad;
    sqrRadius = radius * radius;
    pos[0] = x; pos[1] = y; pos[2] = z;
    bbox[0] = bbox[1] = bbox[2] = rad/2.0;
    material = new Material();
  }

  Sphere(double x, double y, double z, double rad, Material *m) {
      radius = rad;
      sqrRadius = radius * radius;
      pos[0] = x; pos[1] = y; pos[2] = z;
      bbox[0] = bbox[1] = bbox[2] = rad/2.0;
      material = m;
    }

  void normalAtP(const double *p, double *n) {
    blas3dSubstractXY(p, pos, n);
    blas3DNormalize(n);
  }

  double intersect(Ray *r) {
    double oc[3];
    blas3dSubstractXY(pos, r->pos, oc);
    double l2oc = blas3dDot(oc, oc);
    double tca = blas3dDot(oc, r->dir);
    double l2hc = sqrRadius - l2oc + tca*tca;
    if(l2oc < sqrRadius) return tca + sqrt(l2hc);
    else
      if(tca < 0) return std::numeric_limits<double>::infinity();
      else return l2hc > 0 ?
          tca - sqrt(l2hc) : std::numeric_limits<double>::infinity();
  }
};
