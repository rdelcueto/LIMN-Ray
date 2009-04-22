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

/**
 * @class Light
 *
 * @brief This class describes a source of light.
 */
class Light {

public:

  /** An RGB vector describing the color of the source light in the RGB model.*/
  float color[3];
  /** The intensity scale of the light source. */
  float intensity;
  /** The damping scale of the light source. It's meant to compensate the
   * inverse square law of the light source distance, when values grow to big.*/
  float damping;
  /** The vector describing the center/point position of the light source.*/
  float pos[3];
  /** The samples of a light source.*/
  int samples;

  Light() {}

  /**
   * Function that returns the number of samples of a light source.
   * Point lights have a fixed value of 1.
   */
  virtual int getSamples() {return 0;}

  /**
   * Gets the position vector of the i sample of the source light.
   */
  virtual void getPosI(int i, float *lpos) {};

};

typedef std::list<Light*> LightList;

#endif /* LIGHTS_H_ */
