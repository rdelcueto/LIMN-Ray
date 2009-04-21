/*
 * File: materials.h
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

#ifndef MATERIALS_H_
#define MATERIALS_H_

#include <list>

class Material {

public:

  float color[3];
  float opacy;
  float filter;
  float interior;
  float ambient;
  float reflection;
  float diffuse;
  float specular;
  float specular_Hardness;

  Material() {
    color[0] = 1; color[1] = 1; color[2] = 1;
    opacy = 1.0;
    filter = 0.0;
    interior = 1.0;
    ambient = 0.0;
    reflection = 0.0;
    diffuse = 1.0;
    specular = 1.0;
    specular_Hardness = 64;
  }

  Material(float red, float green, float blue,
      float opacy_in, float filter_in) {
    color[0] = 1; color[1] = 1; color[2] = 1;
    opacy = opacy_in;
    filter = filter_in;
    interior = 1.0;
    ambient = 0.0;
    reflection = 0.0;
    diffuse = 1.0;
    specular = 1.0;
    specular_Hardness = 64;
  }

  Material(float red, float green, float blue,
      float opacy_in, float filter_in,
      float ambient_in, float diffuse_in,
      float specular_in, float specHardness_in,
      float reflection_in, float interior_in) {
    color[0] = red; color[1] = green; color[2] = blue;
    opacy = opacy_in;
    filter = filter_in;
    interior = interior_in;
    ambient = ambient_in;
    reflection = reflection_in;
    diffuse = diffuse_in;
    specular = specular_in;
    specular_Hardness = specHardness_in * 8;
  }
};

typedef std::list<Material*> MaterialList;

#endif /* MATERIALS_H_ */
