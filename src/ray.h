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
  double pos[3];
  double dir[3];
  double t_intersect;
  Primitive *p_intersect;

  Ray();

  Ray(double posx, double posy, double posz,
        double dirx, double diry, double dirz);

  Ray(double dirx, double diry, double dirz);

  void setPos(double posx, double posy, double posz);
  void setDir(double dirx, double diry, double dirz);
  void setLookAt(double lookAtx, double lookAty, double lookAtz);
  void setIntersection(double t, Primitive *p);
  void getPosAtT(double t, double *p);
  void getPosAtInt(double *p);
};

#endif /* RAY_H_ */
