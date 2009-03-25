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

Ray::Ray(double posx, double posy, double posz,
        double dirx, double diry, double dirz) {
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    dir[0] = dirx; dir[1] = diry; dir[2] = dirz;
    blasFast3DNormalize(dir);
    t_intersect = std::numeric_limits<double>::infinity();
    p_intersect = NULL;
}

Ray::Ray(double dirx, double diry, double dirz) {
  pos[0] = pos[1] = pos[2] = 0.0;
  dir[0] = dirx; dir[1] = diry; dir[2] = dirz;
  blasFast3DNormalize(dir);
  t_intersect = std::numeric_limits<double>::infinity();
  p_intersect = NULL;
}

void Ray::setPos(double posx, double posy, double posz) {
  pos[0] = posx; pos[1] = posy; pos[2] = posz;
}

void Ray::setDir(double dirx, double diry, double dirz) {
  dir[0] = dirx; dir[1] = diry; dir[2] = dirz;
  blasFast3DNormalize(dir);
}

void Ray::setLookAt(double lookAtx, double lookAty, double lookAtz) {
  dir[0] = lookAtx; dir[1] = lookAty; dir[2] = lookAtz;
  blas3dSubstractXY(dir, pos, dir);
  blasFast3DNormalize(dir);
}

void Ray::setIntersection(double t, Primitive *p) {
  t_intersect = t;
  p_intersect = p;
}

void Ray::getPosAtT(double t, double *p) {
  blasDcopy(3, dir, 1, p, 1);
  blasDscal(3, t, p, 1);
  p[0] += pos[0]; p[1] += pos[1]; p[2] += pos[2];
}

void Ray::getPosAtInt(double *p) {
  getPosAtT(t_intersect, p);
}
