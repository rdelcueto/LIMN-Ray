/*
 * File: ray.cpp
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

#define NRM_EPS 0.000001
#define DIR_EPS 0.0001
#define COLOR_EPS 0.01
#define REFLECT_INTENSITY 1

#include "ray.h"

Ray::Ray() {
  type = 0;
  weight[0] = weight[1] = weight[2] = 1.0;
  position[0] = position[1] = 0.0; position[2] = -5.0;
  direction[0] = direction[1] = 0.0; direction[2] = 1.0;
  intersect_t = std::numeric_limits<double>::infinity();
  sum_ts = 0;
  intersect_material = NULL;
  r_refrac = 1.0;
}

Ray::Ray(double *pixel_in, double *pos_in,
  double xDelta, double yDelta, double zDelta) {
  type = 0;
  weight[0] = weight[1] = weight[2] = 1.0;
  ray_pixel = pixel_in;
  position[0] = pos_in[0]; position[1] = pos_in[1]; position[2] = pos_in[2];
  setLookAt(xDelta, yDelta, zDelta);
  intersect_t = std::numeric_limits<double>::infinity();
  sum_ts = 0;
  intersect_material = NULL;
  r_refrac = 1.0;
}

Ray::Ray(double *pixel_in, double *pos_in, double *dir_in) {
  type = 0;
  weight[0] = weight[1] = weight[2] = 1.0;
  ray_pixel = pixel_in;
  position[0] = pos_in[0]; position[1] = pos_in[1]; position[2] = pos_in[2];
  direction[0] = dir_in[0]; direction[1] = dir_in[1]; direction[2] = dir_in[2];
  intersect_t = std::numeric_limits<double>::infinity();
  sum_ts = 0;
  intersect_material = NULL;
  r_refrac = 1.0;
}

Ray::Ray(Ray *r, int type_in) {

  Ray parent = *r;

  type = 0;
  ray_pixel = parent.ray_pixel;
  position[0] = parent.intersect_point[0];
  position[1] = parent.intersect_point[1];
  position[2] = parent.intersect_point[2];
  intersect_t = std::numeric_limits<double>::infinity();
  sum_ts = parent.sum_ts;
  intersect_material = NULL;

  // Refraction Ray
  if(type_in == 1) {

    double nRatio = parent.intersect_material->interior;
    double n[3];
    blasDcopy(3, parent.direction, 1, direction, 1);
    blasDcopy(3, parent.intersect_normal, 1, n, 1);
    if(blas3dDot(n, direction) < 0) {
      r_refrac = nRatio;
      nRatio = parent.r_refrac/nRatio;
    }
    else {
      r_refrac = parent.r_refrac;
      nRatio = nRatio/parent.r_refrac;
      blasInvert(3, n, n);
    }
    double cosI = blas3dDot(n, direction);
    double sinT2 = nRatio*nRatio * (1 - cosI*cosI);
    if(sinT2 > 1) {
      type_in = 2; // Total Internal Reflection
    }
    else {

      blasDscal(3, nRatio, direction, 1);
      blasDscal(3, nRatio + sqrt(1.0 - sinT2), n, 1);
      blas3dSubstractXY(direction, n, direction);
      blas3DNormalize(direction);

      move2Dir();

      if(parent.weight[0] + parent.weight[1] + parent.weight[2] < COLOR_EPS)
        weight[0] = weight[1] = weight[2] = 0.0;
      else {
        double opacy = 1 - parent.intersect_material->color[3];
        weight[0] = opacy * parent.weight[0];
        weight[1] = opacy * parent.weight[1];
        weight[2] = opacy * parent.weight[2];
      }
    }
  }

  // Reflection Ray
  if(type_in == 2) {
    r_refrac = parent.r_refrac;
    blas3Dcopy(parent.intersect_normal, direction);
    blasDscal(3, -2*blas3dDot(direction, parent.intersect_normal), direction, 1);
    blas3dSubstractXY(parent.direction, direction, direction);
    blas3DNormalize(direction);

    move2IntNrm();

    if(parent.weight[0] + parent.weight[1] + parent.weight[2] < COLOR_EPS)
      weight[0] = weight[1] = weight[2] = 0.0;
    else {
      weight[0] = parent.intersect_material->reflection*
        parent.intersect_material->color[3]*parent.weight[0];
      weight[1] = parent.intersect_material->reflection*
        parent.intersect_material->color[3]*parent.weight[1];
      weight[2] = parent.intersect_material->reflection*
        parent.intersect_material->color[3]*parent.weight[2];
    }
  }
}

void Ray::move2IntNrm() {
  position[0] += direction[0]*NRM_EPS;
  position[1] += direction[1]*NRM_EPS;
  position[2] += direction[2]*NRM_EPS;
}

void Ray::move2Dir() {
  position[0] += direction[0]*DIR_EPS;
  position[1] += direction[1]*DIR_EPS;
  position[2] += direction[2]*DIR_EPS;
}

void Ray::move(double delta) {
  position[0] += direction[0]*delta;
  position[1] += direction[1]*delta;
  position[2] += direction[2]*delta;
}

void Ray::setLookAt(double *lookAt) {
  direction[0] = lookAt[0]; direction[1] = lookAt[1]; direction[2] = lookAt[2];
  blas3dSubstractXY(direction, position, direction);
  blas3DNormalize(direction);
}

void Ray::setLookAt(double lookAtx, double lookAty, double lookAtz) {
  direction[0] = lookAtx; direction[1] = lookAty; direction[2] = lookAtz;
  blas3dSubstractXY(direction, position, direction);
  blas3DNormalize(direction);
}

void Ray::getPosAtT(double t, double *p) {
  blasDcopy(3, direction, 1, p, 1);
  blasDscal(3, t, p, 1);
  p[0] += position[0]; p[1] += position[1]; p[2] += position[2];
}
