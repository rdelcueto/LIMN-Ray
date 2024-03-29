/*
 * File: rays.cpp
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

#include "rays.h"

/**
 * @class VisionRay
 *
 * @brief This class represents a vision ray.
 *
 * This class describes the type of rays that will be drawn in the rendered
 * scene. This type of rays carry information such as the pixel from the parent
 * primary ray to which this ray is related in the rendered scene and ZBuffer.
 * It also stores the refraction index of the parent ray, in order to detect
 * different ray's refraction.
 * \attention {This needs to be changed into a linked list in order to support
 * multilevel contention of primitives.}
 */
class VisionRay : public Ray {
public:

  /** Pointer to the related pixel in the rendered image.*/
  float *pixel;
  /** Pointer to the related pixel in the ZBuffer image.*/
  float *zBufferPixel;
  /** Value of the refraction index from the parent ray.*/
  float startRefractIndex;

  VisionRay(float *pixel_in, float *zBufferPixel_in, float *pos) {
    type = 0;
    sumTs = 0;
    pixel = pixel_in;
    zBufferPixel = zBufferPixel_in;
    weight[0] = weight[1] = weight[2] = 1.0;
    blasCopy(pos, position);
    startRefractIndex = 1.0;
    intersectionT = std::numeric_limits<float>::infinity();
  }

  VisionRay(VisionRay *r, int rayType) {
    type = 0;
    sumTs = r->sumTs;
    pixel = r->pixel;
    zBufferPixel = NULL;
    startRefractIndex = r->intersectionMaterial->interior;
    intersectionT = std::numeric_limits<float>::infinity();
    blasCopy(r->intersectionPoint, position);

    float n[3];
    blasCopy(r->direction, direction);
    blasCopy(r->intersectionNormal, n);
    float cosT1 = blasDot(n, direction);

    // Refraction Rays
    if(rayType == 1) {
      float alpha = 1 - r->intersectionMaterial->opacy;
      float filter[3];
      filter[0] = filter[1] = filter[2] =  r->intersectionMaterial->filter;
      filter[0] *= 1 - r->intersectionMaterial->color[0];
      filter[1] *= 1 - r->intersectionMaterial->color[1];
      filter[2] *= 1 - r->intersectionMaterial->color[2];

      float nRatio = r->intersectionMaterial->interior;

      if(nRatio != 1.0) {
        if(cosT1 < 0)
          nRatio = 1/nRatio;
        else
          blasInvert(n, n);

        float sinT2 = nRatio*nRatio * (1 - cosT1*cosT1);

        if(sinT2 > 1) {
          // Total Reflection
          weight[0] = weight[1] = weight[2] = 0.0;
        }
        else {
          float cosT2 = sqrt(1 - sinT2);
          blasScale(direction, nRatio, direction);
          if(cosT1 > 0)
            blasScale(n, nRatio*cosT1 - cosT2, n);
          else
            blasScale(n, nRatio*cosT1 + cosT2, n);
          blasSubstract(direction, n, direction);
          blasNormalize(direction);
        }
      }

      weight[0] = r->weight[0] * (alpha - filter[0]);
      weight[1] = r->weight[1] * (alpha - filter[1]);
      weight[2] = r->weight[2] * (alpha - filter[2]);

      checkPositiveWeight();
      move2Dir();
    }

    // Reflection rays
    if(rayType == 2) {
      startRefractIndex = r->startRefractIndex;
      blasScale(n, 2.0*blasDot(r->intersectionNormal, direction), n);
      blasSubstract(direction, n, direction);
      blasNormalize(direction);

      float reflectiveAlpha = r->intersectionMaterial->reflection;
      weight[0] = r->weight[0] * reflectiveAlpha;
      weight[1] = r->weight[1] * reflectiveAlpha;
      weight[2] = r->weight[2] * reflectiveAlpha;

      checkPositiveWeight();
      move2Vec(r->intersectionNormal);
    }
  }
};

/**
 * @class ShadowRay
 *
 * @brief This class represents a shadow ray.
 *
 * This class is meant to create lightweight version of rays, which are used
 * just to detect the amount of light from each source light.
 *
 * Each shadow ray travels to a sample light and detects intersections on its
 * way, using the information of each intersected object's material to
 * calculate the intensity and color of that light source.
 */
class ShadowRay : public Ray {
public:

  ShadowRay(float *pos, float *dir) {
    type = -1;
    sumTs = 0;
    blasCopy(pos, position);
    blasCopy(dir, direction);
    weight[0] = weight[1] = weight[2] = 1.0;
    intersectionT = std::numeric_limits<float>::infinity();
  }

  /**
   * This function calculates the next step on the way of the ray to the light
   * source iterating through each intersection in each call.
   */
  void nextStep() {
    sumTs += intersectionT;
    blasCopy(intersectionPoint, position);
    intersectionT = std::numeric_limits<float>::infinity();
    // Shadow Opacy + Color Filtering
    float alpha = 1 - intersectionMaterial->opacy;
    float filter[3];
    filter[0] = filter[1] = filter[2] =  intersectionMaterial->filter;
    filter[0] *= 1 - intersectionMaterial->color[0];
    filter[1] *= 1 - intersectionMaterial->color[1];
    filter[2] *= 1 - intersectionMaterial->color[2];
    weight[0] = weight[0] * (alpha - filter[0]);
    weight[1] = weight[1] * (alpha - filter[1]);
    weight[2] = weight[2] * (alpha - filter[2]);

    checkPositiveWeight();
    move2Dir();
  }
};
