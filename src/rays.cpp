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

class VisionRay : public Ray {
public:
  double *pixel;
  double weight[3];
  double startRefractIndex;

  VisionRay(double *pixel_in, double *pos, double *dir) {
    type = 0;
    sumTs = 0;
    pixel = pixel_in;
    weight[0] = weight[1] = weight[2] = 1.0;
    blasCopy(pos, position);
    blasCopy(dir, direction);
    startRefractIndex = 1.0;
    intersectionT = std::numeric_limits<double>::infinity();
  }

  VisionRay(double *pixel_in, double *pos) {
    type = 0;
    sumTs = 0;
    pixel = pixel_in;
    weight[0] = weight[1] = weight[2] = 1.0;
    blasCopy(pos, position);
    startRefractIndex = 1.0;
    intersectionT = std::numeric_limits<double>::infinity();
  }

  VisionRay(VisionRay *r, int rayType) {
    type = 0;
    sumTs = r->sumTs;
    pixel = r->pixel;
    startRefractIndex = r->intersectionMaterial->interior;
    intersectionT = std::numeric_limits<double>::infinity();
    blasCopy(r->intersectionPoint, position);

    double n[3];
    blasCopy(r->direction, direction);
    blasCopy(r->intersectionNormal, n);
    double cosT1 = blasDot(n, direction);

    // Refraction Rays
    if(rayType == 1) {
      double alpha = 1 - r->intersectionMaterial->opacy;
      double filter[3];
      filter[0] = filter[1] = filter[2] =  r->intersectionMaterial->filter;
      filter[0] *= 1 - r->intersectionMaterial->color[0];
      filter[1] *= 1 - r->intersectionMaterial->color[1];
      filter[2] *= 1 - r->intersectionMaterial->color[2];

      double nRatio = r->intersectionMaterial->interior;

      if(cosT1 < 0)
        nRatio = 1/nRatio;
      else
        blasInvert(n, n);

      double sinT2 = nRatio*nRatio * (1 - cosT1*cosT1);

      if(sinT2 > 1) {
        // Total Reflection
        weight[0] = weight[1] = weight[2] = 0.0;
      }
      else {
        double cosT2 = sqrt(1 - sinT2);
        blasScale(direction, nRatio, direction);
        if(cosT1 > 0)
          blasScale(n, nRatio*cosT1 - cosT2, n);
        else
          blasScale(n, nRatio*cosT1 + cosT2, n);
        blasSubstract(direction, n, direction);
        blasNormalize(direction);
      }

      weight[0] = r->weight[0] * (alpha - filter[0]);
      weight[1] = r->weight[1] * (alpha - filter[1]);
      weight[2] = r->weight[2] * (alpha - filter[2]);

      move2Dir();
    }

    // Reflection rays
    if(rayType == 2) {
      startRefractIndex = r->startRefractIndex;
//      if(cosT1 < 0) {
//        blasScale(n, 2.0*cosT1, n);
//      }
//      else {
//        blasInvert(n, n);
//        blasScale(n, -2.0*cosT1, n);
//      }
      blasScale(n, 2.0*blasDot(r->intersectionNormal, direction), n);
      blasSubstract(direction, n, direction);
      blasNormalize(direction);

      double reflectiveAlpha = r->intersectionMaterial->reflection;
      weight[0] = r->weight[0] * reflectiveAlpha;
      weight[1] = r->weight[1] * reflectiveAlpha;
      weight[2] = r->weight[2] * reflectiveAlpha;

      move2Vec(r->intersectionNormal);
    }
  }

  double getWeight() {
    return (fabs(weight[0]) + fabs(weight[1]) + fabs(weight[2]));
  }
};

class ShadowRay : public Ray {
public:
  ShadowRay(double *pos, double *dir) {
    type = -1;
    sumTs = 0;
    blasCopy(pos, position);
    blasCopy(dir, direction);
    intersectionT = std::numeric_limits<double>::infinity();
  }

  void nextStep() {
    blasCopy(intersectionPoint, position);
    move2Dir();
  }
};
