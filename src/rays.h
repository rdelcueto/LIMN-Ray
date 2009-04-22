/*
 * File: rays.h
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

#ifndef RAYS_H_
#define RAYS_H_

#include <limits>
#include "blas.h"
#include "materials.h"

#define COLOR_EPS 0.01

/**
 * @class Ray
 *
 * @brief This class describes a ray.
 *
 * The class Ray describes any ray by using three 3D vectors:
 *      Position: The point in space were the ray starts.
 *      Direction: The direction in which the ray travels.
 *      Weight: An RGB vector that represents how much light in each channel
 *              will result from that ray.
 *
 * The Ray class contains more information relative to the ray in case it has
 * been checked and resulted in positive intersection, such as it's
 * intersection point, the normal of the intersected surface at the intersection
 * point, the t parameter at the moment of intersection and a pointer to the
 * material of the object being intersected.
 */

class Ray {

public:
  /** Ray type flag: Its states if a ray produces a new generation of
   * rays, and what type of rays are these (Reflection and/or refraction).*/
  int type;
  /** The accumulative distance since the primary parent ray. */
  float sumTs;
  /** The 3D vector describing the start point for the ray.*/
  float position[3];
  /** The 3D vector describing the direction of the ray.*/
  float direction[3];
  /** The 3D vector describing the RGB weight of the ray.*/
  float weight[3];
  /** The size of the direction vector at the moment of intersection.*/
  float intersectionT;
  /** The 3D vector describing the point in space of intersection.*/
  float intersectionPoint[3];
  /** The 3D vector describing the normal of the surface at intersection.*/
  float intersectionNormal[3];
  /** The material of the object being intersected.*/
  Material *intersectionMaterial;

  /**
   * Function that moves the vector's starting point an epsilon towards the
   * vector v.
   *
   * This is used in reflection rays to avoid the "acne" artifacts caused by
   * numeric errors, since the ray's starting point is it's parent intersection
   * point.
   *
   * @param v Direction vector.
   */
  void move2Vec(float *v) {
    position[0] += v[0]*EPS_2;
    position[1] += v[1]*EPS_2;
    position[2] += v[2]*EPS_2;
  }

  /**
   * Function that moves the vector's starting point an epsilon towards the
   * normal of the intersecting surface #intersectionNormal.
   *
   * This is used in refraction/transparency/shadow rays to avoid the "acne"
   * artifacts caused by numeric errors, since the ray's starting point is it's
   * parent intersection point.
   */
  void move2Dir() {
    position[0] += direction[0]*EPS_3;
    position[1] += direction[1]*EPS_3;
    position[2] += direction[2]*EPS_3;
  }

  /**
   * This function helps create a direction vector for the ray using an
   * arbitrary 3D vector in space.
   *
   * @param lookAt 3D vector in space.
   */
  void setLookAt(float *lookAt) {
    blasSubstract(lookAt, position, direction);
    blasNormalize(direction);
  }

  /**
   * This function helps create a direction vector for the ray using an
   * arbitrary point in space.
   *
   * This is used when creating the primary rays using the projection plane's
   * sampling grid.
   *
   * @param xlookAt x-axis coordinate of the LookAt 3D vector.
   * @param ylookAt y-axis coordinate of the LookAt 3D vector.
   * @param zlookAt z-axis coordinate of the LookAt 3D vector.
   */
  void setLookAt(float xlookAt, float ylookAt, float zlookAt) {
    direction[0] = xlookAt; direction[1] = ylookAt; direction[2] = zlookAt;
    blasSubstract(direction, position, direction);
    blasNormalize(direction);
  }

  /**
   * This function returns the value of the total weight of the ray.
   */
  float getWeight() {
    return (weight[0] + weight[1] + weight[2]);
  }

  /**
   * This function checks the ray's weight for positive values and resets them
   * to zero in case they have a negative value.
   */
  void checkPositiveWeight() {
    weight[0] = weight[0] < 0 ? 0 : weight[0];
    weight[1] = weight[1] < 0 ? 0 : weight[1];
    weight[2] = weight[2] < 0 ? 0 : weight[2];
  }

  /**
   * Gets the position of the ray when moving in the ray's direction a t
   * from the ray's starting point.
   *
   * @param t The distance from the starting point.
   * @param p The vector at t distance in the rays direction.
   */
  void getPosAtT(float t, float *p) {
    blasScale(direction, t, p);
    blasAdd(position, p, p);
  }
};

#endif /* RAYS_H_ */
