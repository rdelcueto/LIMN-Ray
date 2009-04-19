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

class Primitive {

public:

  float pos[3];
  Material *material;

  Primitive() {}

  virtual float intersect(Ray *r) { return 0; }
  virtual void normalAtP(const float *p, float *n) {}
  virtual void getPos(float *pos) {}

};

typedef std::list<Primitive*> PrimitiveList;

#endif /* PRIMITIVES_H_ */
