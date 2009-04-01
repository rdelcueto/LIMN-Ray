/*
 * File: materials.h
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

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <list>

class Material {

public:

  double color[4];
  double ambient;
  double diffuse;
  double specular;
  double specular_Hardness;
  double reflection;
  double interior;

  Material() {
    color[0] = 1; color[1] = 1; color[2] = 1; color[3] = 0.5;
    ambient = 0;
    diffuse = 0.9;
    specular = 1.0;
    specular_Hardness = 16 * 4;
    reflection = 0;
    interior = 0;
  }

  Material(double red, double green, double blue, double opacy) {
    color[0] = red; color[1] = green; color[2] = blue; color[3] = opacy;
    ambient = 0;
    diffuse = 0.9;
    specular = 1.0;
    specular_Hardness = 16 * 4;
    reflection = 0;
    interior = 0;
  }

  Material(double red, double green, double blue, double opacy,
           double ambient_in, double diffuse_in,
           double specular_in, double specHardness_in,
           double reflection_in, double interior_in) {
    color[0] = red; color[1] = green; color[2] = blue; color[3] = opacy;
    ambient = ambient_in;
    diffuse = diffuse_in;
    specular = specular_in;
    specular_Hardness = specHardness_in * 4; // Blinn Half-way vector compensate
    reflection = reflection_in;
    interior = interior_in;
  }

};

typedef std::list<Material*> MaterialList;

#endif /* MATERIAL_H_ */
