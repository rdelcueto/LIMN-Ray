/*
 * File: lights.h
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

#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <list>
#include "blas.h"

class Light {

public:

  float color[3];
  float intensity;
  float damping;

  float pos[3];
  int samples;
  float *samplePos;

  Light() {}

  virtual int getSamples() {return 0;}
  virtual void getPosI(int i, float *lpos) {};

};

typedef std::list<Light*> LightList;

#endif /* LIGHTS_H_ */
