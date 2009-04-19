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
  float sumTs;
  float position[3];
  float direction[3];
  float weight[3];
  float intersectionT;
  float intersectionPoint[3];
  float intersectionNormal[3];
  Material *intersectionMaterial;

  void move2Vec(float *v) {
    position[0] += v[0]*EPS_2;
    position[1] += v[1]*EPS_2;
    position[2] += v[2]*EPS_2;
  }

  void move2Dir() {
    position[0] += direction[0]*EPS_3;
    position[1] += direction[1]*EPS_3;
    position[2] += direction[2]*EPS_3;
  }

  void setLookAt(float *lookAt) {
    blasSubstract(lookAt, position, direction);
    blasNormalize(direction);
  }

  void setLookAt(float xlookAt, float ylookAt, float zlookAt) {
    direction[0] = xlookAt; direction[1] = ylookAt; direction[2] = zlookAt;
    blasSubstract(direction, position, direction);
    blasNormalize(direction);
  }

  float getWeight() {
    return (weight[0] + weight[1] + weight[2]);
  }

  void checkPositiveWeight() {
    weight[0] = weight[0] < 0 ? 0 : weight[0];
    weight[1] = weight[1] < 0 ? 0 : weight[1];
    weight[2] = weight[2] < 0 ? 0 : weight[2];
  }

  void getPosAtT(float t, float *p) {
    blasScale(direction, t, p);
    blasAdd(position, p, p);
  }
};

#endif /* RAYS_H_ */
