/*
 * File: ray.h
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

#ifndef RAY_H_
#define RAY_H_

#include "blas.h"

class Primitive;

class Ray
{
public:
  int type;
  double colorWeight;
  double *pixel;
  double pos[3];
  double dir[3];
  double int_p[3];
  double int_n[3];
  double t_intersect;
  Primitive *p_intersect;

  Ray();
  Ray(double *pixel_in, double *pos_in,
      double xDelta, double yDelta, double zDelta);
  Ray(double *pixel_in, double *pos_in, double *dir_in);
  Ray(Ray *r, int type);

  void setPos(double *pos);
  void setDir(double *dir);
  void move2IntNrm();
  void move2Dir();
  void move(double delta);
  void setLookAt(double *lookAt);
  void setLookAt(double lookAtx, double lookAty, double lookAtz);
  void getPosAtT(double t, double *p);
};

#endif /* RAY_H_ */
