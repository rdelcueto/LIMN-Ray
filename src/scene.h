/*
 * File: scene.h
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


#ifndef SCENE_H_
#define SCENE_H_

#define INTERSECT_EPSILON 0.001
#define LIGHT_SCALE 10000

#include <fstream>
#include <omp.h>
#include <pngwriter.h>

#include "primitives.cpp"
#include "materials.cpp"
#include "lights.cpp"
#include "rays.cpp"

class Scene  {

public:

  int image_width;
  int image_height;
  int res;

  double cameraPos[3];
  double cameraLookAt[3];
  double focalLength;

  int samplesPerPixel;
  int secondaryRaysDepth;
  int shadows;
  int saveZbuffer;
  int zBufferMaxDepth;

  double *renderedImage;
  double *zBuffer;

  PrimitiveList sceneObjects;
  MaterialList sceneMaterials;
  LightList sceneLights;

  Scene();
  void render();
  int raytrace(VisionRay **rays, int nRays, double *zbuffer);
  void buildSecondaryRays(VisionRay **oldRays, VisionRay **newRays, int nRays);
  void deleteRayArray(VisionRay **rays, int nRays);
  double intersectRay(Ray *r);
  int shadeRayIntersection(VisionRay *r);
  void outputImage(double* image, int image_w, int image_h, int imageFlag, char* imageName);
  double diffclock(clock_t clock1, clock_t clock2);

};

#endif /* SCENE_H_ */
