/*
 * File: rays.h
 * Author: Rodrigo González del Cueto
 */

/*
 * Copyright (C) 2009 Rodrigo González del Cueto
 *
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

#ifndef RAYS_H_
#define RAYS_H_

#include <limits>
#include "blas.h"
#include "materials.h"

#define COLOR_EPS 0.01

class Ray {

public:
  int type;
  double sumTs;
  double position[3];
  double direction[3];
  double intersectionT;
  double intersectionPoint[3];
  double intersectionNormal[3];
  Material *intersectionMaterial;

  void move2Vec(double *v) {
    position[0] += v[0]*EPS_2;
    position[1] += v[1]*EPS_2;
    position[2] += v[2]*EPS_2;
  }

  void move2Dir() {
    position[0] += direction[0]*EPS_3;
    position[1] += direction[1]*EPS_3;
    position[2] += direction[2]*EPS_3;
  }

  void setLookAt(double *lookAt) {
    blasSubstract(lookAt, position, direction);
    blasNormalize(direction);
  }

  void setLookAt(double xlookAt, double ylookAt, double zlookAt) {
    direction[0] = xlookAt; direction[1] = ylookAt; direction[2] = zlookAt;
    blasSubstract(direction, position, direction);
    blasNormalize(direction);
  }

  void getPosAtT(double t, double *p) {
    blasScale(direction, t, p);
    blasAdd(position, p, p);
  }
};

#endif /* RAYS_H_ */
