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

/**
 * @class Material
 *
 * @brief This class describes an objects material.
 *
 * In this class we model a basic material definition. It includes variables
 * describing: Color, transparency and filtering of the material together
 * with the variables needed by the Phong model: Diffuse scale, specular scale
 * and specular hardness index.
 */
class Material {

public:

  /** An RGB vector representing the color of the material in the RGB model. */
  float color[3];
  /** The opacy index of the material. It represents how much light goes
   * through it.*/
  float opacy;
  /** The filter index of the material. It represents how much of the light
   * that goes through it is filtered by the #color of the material.
   * A material with filtering, apply a color mask to the shadow/secondary rays
   * that intersect it.*/
  float filter;
  /** The interior or refraction index of the material. It's used when the
   * material has the #opacy index < 1, to calculate the angle in which the new
   * rays will be transformed due to light change of direction.*/
  float interior;
  /** The ambient of the material. It's used to give ambient light to the
   * material by simply adding this value multiplied by the #color in the
   * shading function.*/
  float ambient;
  /** This is the Phong's ambient constant value. It represents the ratio of
   * reflection of the ambient term present in all points in the scene
   * rendered.*/
  float reflection;
  /** This is the Phong's diffuse constant value. It represents the ratio of
   * reflection of the diffuse term of the incoming light. */
  float diffuse;
  /** This is the Phong's specular constant value. It represents the ratio of
     * reflection of the specular term of the incoming light. */
  float specular;
  /** This is the Phong's specular hardness/shininess for this material,
   * which is larger for surfaces that are smoother and more mirror-like.
   * When this constant is large the specular highlight is small*/
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
    specular_Hardness = 16;
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
    specular_Hardness = 16;
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
    specular_Hardness = specHardness_in;
  }
};

typedef std::list<Material*> MaterialList;

#endif /* MATERIALS_H_ */
