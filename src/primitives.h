/*
 * File: primitive.h
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

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include <list>
#include "blas.h"
#include "rays.h"
#include "materials.h"

/**
 * @class Primitive
 *
 * @brief This class describes a primitive object.
 *
 * This is class describes any object, it contains basic information of every
 * object in the scene, such as its position in space and it's material.
 */
class Primitive {

public:

  /** The position vector of the object.*/
  float pos[3];
  /** The pointer to the Material it has been asigned */
  Material *material;

  Primitive() {}

  /**
   * This function implements the intersection algorithm for the given class
   * of object. The function receives a Ray as argument and checks for
   * intersection. In case of intersection, the function returns the size of
   * the direction vector of the ray at the point of intersection. If the
   * ray doesn't intersect, it returns infinity.
   *
   * @param r Ray to be checked for intersection.
   * @result The t parameter for the ray.
   */
  virtual float intersect(Ray *r) { return 0; }

  /**
   * This function gets the normal of the object at a given point of it's
   * surface.
   *
   * @param p 3D vector describing the point at the object's surface.
   * @param n The vector describing the normal to the surface.
   */
  virtual void normalAtP(const float *p, float *n) {}

  /**
   * This function gets the object's position vector.
   *
   * @param pos The destination for the position vector.
   */
  virtual void getPos(float *pos) {}

};

typedef std::list<Primitive*> PrimitiveList;

#endif /* PRIMITIVES_H_ */
