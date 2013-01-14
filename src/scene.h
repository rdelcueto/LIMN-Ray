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

#define INF_LIMIT std::numeric_limits<float>::infinity()
#define INTERSECT_EPSILON 0.001
#define LIGHT_SCALE 10000

#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <omp.h>
#include <Magick++.h>

#include "primitives.cpp"
#include "materials.cpp"
#include "lights.cpp"
#include "rays.cpp"

/**
 * @class Scene
 *
 * @brief Main class describing a LIMN-Ray scene.
 *
 * This class is the main container of a scene, all the variables describing
 * a render are declared in this class together with all the functions
 * implementing the raytracing and lighting/shading algorithms.
 *
 */
class Scene  {

public:

  /** Number of Threads. */
  int num_threads;
  /** String containing the output filename. */
  std::string fileOut;
  /** Width in pixels of the rendered image. */
  int image_width;
  /** Height in pixels of the rendered image. */
  int image_height;
  /** Rendered image resolution Width x Height. */
  int res;
  /** Camera up vector rotation angle. */
  float cameraRollAngle;
  /** Camera position vector. */
  float cameraPos[3];
  /** Camera look at vector. */
  float cameraLookAt[3];
  /** Projection focal point position. */
  float fPoint[3];
  /** Distance from the projection plane. */
  float focalLength;
  /** Z-Buffer's zero point. */
  float focusDistance;
  /** Sqrt of the total samples per pixel. */
  int sqrtSamplesPerPixel;
  /** Recursive maximum ray depth. */
  int secondaryRaysDepth;
  /** Enable shadows flag. (0 = disabled) */
  int shadows;
  /** Enable Z-Buffer output. (0 = disabled) */
  int saveZBuffer;
  /** Z-Buffer normalization value. */
  int zBufferMaxDepth;
  /** Rendered image pixel array. */
  float *renderedImage;
  /** Z-Buffer pixel array. */
  float *zBuffer;


  /** Scene's primitives linked list. */
  PrimitiveList sceneObjects;
  /** Scene's materials linked list. */
  MaterialList sceneMaterials;
  /** Scene's lights linked list. */
  LightList sceneLights;
  /** 3x3 Ray camera rotation matrix. */
  float rayTransformationMat[9];

  /**
   * Scene class empty constructor.
   * Initializes all variables with non zero/NULL values.
   */
  Scene();

  /**
   * Initializes the scene's variables to describe the benchmarkScene
   * configuration.
   */
  void benchmarkScene();

  /**
   * This function performs the raytracer's main loop in the image array pixels.
   * Once all the values of a scene have been specified, by calling the render
   * function, the program will initialize the rendering process.
   * First it creates the primary rays using the Scene values, and sends them
   * to the raytrace() function to be processed. When the raytrace() function
   * returns, it loops calling buildSecondaryRays() until there are no more rays
   * or the #secondaryRaysDepth limit is reached.
   * rendering process is complete, it will output the rendered image arrays
   * to a PNG image file with the name #fileOut + ".png" or "image_out.png" in
   * case it wasn't specified at runtime.
   */
  void render();

  /**
   * This function performs the basic raytracing algorithm.
   * The render() function creates clusters of rays stored in a VisionRay array
   * rays and sent to this function in order to be processed. After each ray
   * has been processed it saves whether the ray produces a secondary ray
   * (reflection/refraction/transparency) and after processing all the whole
   * ray array it returns the number of sum of secondary rays.
   *
   * @param rays Array of rays to be processed.
   * @param nRays size of the rays array.
   *
   * @return The value of the sum of secondary rays produced by the rays array.
   */
  int raytrace(VisionRay **rays, int nRays);

  /**
   * This function receives a VisionRay array, and process it to create a
   * second VisionRay array with the new generation of rays produces by the
   * old rays.
   *
   * @param oldRays Array of rays to be processed.
   * @param newRays Array of new generation of rays.
   * @param nRays size of the oldRays array.
   */
  void buildSecondaryRays(VisionRay **oldRays, VisionRay **newRays, int nRays);

  /**
   * This function iterates through a VisionRay array and deletes the objects
   * stored in it.
   *
   * @param rays Array of rays to be deleted.
   * @param nRays size of the rays array.
   */
  void deleteRayArray(VisionRay **rays, int nRays);

  /**
   * This function receives a ray, and checks if it intersects with any of the
   * objects in the scene. After going through the data structure, its gets the
   * first of the intersections and stores the information inside the Ray.
   *
   * If it doesn't intersect any object, it sets the ray's Ray::intersection_t value
   * to infinity.
   *
   * @param r Ray to be tested for intersection.
   */
  void intersectRay(Ray *r);

  /**
   * This function receives a VisionRay after it has intersected with an object
   * and it calculates the resulting shading ray's assigned pixel.
   *
   * The lighting models used are the Phong diffuse light model and a specular
   * approximation using the halfway vector instead of actual reflection vector.
   *
   * @ param r Ray to be shaded.
   * @return The number of secondary rays produces by the r based on the
   * intersected object's material.
   */
  int shadeRayIntersection(VisionRay *r);

  /**
   * This function outputs the Scene #renderedImage array, as well as the
   * #zBuffer array if enabled, into a Png image file.
   *
   * The function uses the PngWriter library to create and write the image
   * files.
   *
   * @param imageName The string (without extension) for the output image
   * filename.
   */
  void outputImage(std::string imageName);

};

#endif /* SCENE_H_ */
