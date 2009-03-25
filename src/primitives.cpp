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
    blas3dSubstractXY(pos, p, n);
    blasFast3DNormalize(n);
  }

  double intersect(Ray *r) {
    double oc[3];
    blas3dSubstractXY(pos, r->pos, oc);
    double l2oc = blasFast3dDot(oc, oc);
    double tca = blasFast3dDot(oc, r->dir);
    double l2hc = sqrRadius - l2oc + tca*tca;
    if(tca < 0) {
      std::cout << tca << " ";
      if(l2hc < 0) return std::numeric_limits<double>::infinity();
      else return tca - sqrt(l2hc);
    }
    else
      return tca + sqrt(l2hc);
  }
};
