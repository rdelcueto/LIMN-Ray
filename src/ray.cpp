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

#include "ray.h"

Ray::Ray() {
  pos[0] = pos[1] = 0.0; pos[2] = -5.0;
  dir[0] = dir[1] = 0.0; dir[2] = 1.0;
  t_intersect = std::numeric_limits<double>::infinity();
  p_intersect = NULL;
}

Ray::Ray(double *pixel_in, double *pos_in,
  double xDelta, double yDelta, double zDelta) {
  pixel = pixel_in;
  pos[0] = pos_in[0]; pos[1] = pos_in[1]; pos[2] = pos_in[2];
  setLookAt(xDelta, yDelta, zDelta);
  t_intersect = std::numeric_limits<double>::infinity();
  p_intersect = NULL;
}

Ray::Ray(double *pixel_in, double *pos_in, double *dir_in) {
  pixel = pixel_in;
  pos[0] = pos_in[0]; pos[1] = pos_in[1]; pos[2] = pos_in[2];
  dir[0] = dir_in[0]; dir[1] = dir_in[1]; dir[2] = dir_in[2];
  t_intersect = std::numeric_limits<double>::infinity();
  p_intersect = NULL;
}

void Ray::setPos(double *pos_in) {
  pos[0] = pos_in[0]; pos[1] = pos_in[1]; pos[2] = pos_in[2];
}

void Ray::setDir(double *dir_in) {
  dir[0] = dir_in[0]; dir[1] = dir_in[1]; dir[2] = dir_in[2];
  //blasFast3DNormalize(dir);
}

void Ray::move(double delta) {
  pos[0] += dir[0]*delta;
  pos[1] += dir[1]*delta;
  pos[2] += dir[2]*delta;
}

void Ray::setLookAt(double *lookAt) {
  dir[0] = lookAt[0]; dir[1] = lookAt[1]; dir[2] = lookAt[2];
  blas3dSubstractXY(dir, pos, dir);
  blas3DNormalize(dir);
}

void Ray::setLookAt(double lookAtx, double lookAty, double lookAtz) {
  dir[0] = lookAtx; dir[1] = lookAty; dir[2] = lookAtz;
  blas3dSubstractXY(dir, pos, dir);
  blas3DNormalize(dir);
}

void Ray::getPosAtT(double t, double *p) {
  blasDcopy(3, dir, 1, p, 1);
  blasDscal(3, t, p, 1);
  p[0] += pos[0]; p[1] += pos[1]; p[2] += pos[2];
}
