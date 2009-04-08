/*
 * File: scene.h
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

#ifndef SCENE_H_
#define SCENE_H_

#include "primitives.h"
#include "materials.h"
#include "lights.h"

class Scene {
public:

  int plane_w;
  int plane_h;

  double planePos[3];
  double focalPointPos[3];
  double lookAt[3];

  int recurLimit;
  int samples;

  PrimitiveList sceneObjects;
  MaterialList sceneMaterials;
  LightList sceneLights;

  Scene();
  double intersect(Ray *r);
  int shader(Ray *r);

};

#endif /* SCENE_H_ */
