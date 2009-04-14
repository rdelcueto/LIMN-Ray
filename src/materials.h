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

  double color[3];
  double opacy;
  double filter;
  double interior;
  double ambient;
  double reflection;
  double diffuse;
  double specular;
  double specular_Hardness;

  Material() {
    color[0] = 1; color[1] = 1; color[2] = 1;
    opacy = 1.0;
    filter = 0.0;
    interior = 1.0;
    ambient = 0.0;
    reflection = 0.0;
    diffuse = 1.0;
    specular = 1.0;
    specular_Hardness = 32;
  }

  Material(double red, double green, double blue,
      double opacy_in, double filter_in) {
    color[0] = 1; color[1] = 1; color[2] = 1;
    opacy = opacy_in;
    filter = filter_in;
    interior = 1.0;
    ambient = 0.0;
    reflection = 0.0;
    diffuse = 1.0;
    specular = 1.0;
    specular_Hardness = 32;
  }

  Material(double red, double green, double blue,
      double opacy_in, double filter_in,
      double ambient_in, double diffuse_in,
      double specular_in, double specHardness_in,
      double reflection_in, double interior_in) {
    color[0] = red; color[1] = green; color[2] = blue;
    opacy = opacy_in;
    filter = filter_in;
    interior = interior_in;
    ambient = ambient_in;
    reflection = reflection_in;
    diffuse = diffuse_in;
    specular = specular_in;
    specular_Hardness = specHardness_in;
  }
};

typedef std::list<Material*> MaterialList;

#endif /* MATERIALS_H_ */
