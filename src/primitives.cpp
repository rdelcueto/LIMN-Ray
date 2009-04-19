/*
 * File: primitives.cpp
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

#include "primitives.h"

class Plane : public Primitive {
public:
  float planeN[3];

  Plane() {
    pos[0] = 0.0; pos[1] = 0.0; pos[2] = 0.0;
    planeN[0] = 0.0; planeN[1] = 1.0; planeN[2] = 0.0;
    material = new Material();
  }

  Plane(float posx, float posy, float posz,
      float normalx, float normaly, float normalz) {
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    planeN[0] = normalx; planeN[1] = normaly; planeN[2] = normalz;
    blasNormalize(planeN);
    material = new Material();
  }

  Plane(float posx, float posy, float posz,
      float normalx, float normaly, float normalz,
      Material *m) {
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    planeN[0] = normalx; planeN[1] = normaly; planeN[2] = normalz;
    blasNormalize(planeN);
    material = m;
  }

  void normalAtP(const float *p, float *n) {
    n[0] = planeN[0]; n[1] = planeN[1]; n[2] = planeN[2];
  }

  float intersect(Ray *r) {
    float nDotDir = blasDot(planeN, r->direction);
    if (nDotDir != 0) {
      float line[3];
      blasSubstract(r->position, pos, line);
      float nDotLine = -blasDot(planeN, line);
      if(nDotLine != 0)
        return nDotLine/nDotDir;
    }
    return std::numeric_limits<float>::infinity();
  }
};

class Sphere : public Primitive {
public:
  float radius;
  float sqrRadius;

  Sphere() {
    radius = 1.0;
    sqrRadius = 1.0;
    pos[0] = 0.0; pos[1] = 0.0; pos[2] = 100.0;
    material = new Material();
  }

  Sphere(float x, float y, float z, float rad) {
    radius = rad;
    sqrRadius = radius * radius;
    pos[0] = x; pos[1] = y; pos[2] = z;
    material = new Material();
  }

  Sphere(float x, float y, float z, float rad, Material *m) {
      radius = rad;
      sqrRadius = radius * radius;
      pos[0] = x; pos[1] = y; pos[2] = z;
      material = m;
    }

  void normalAtP(const float *p, float *n) {
    blasSubstract(p, pos, n);
    blasNormalize(n);
  }

  float intersect(Ray *r) {
    float oc[3];
    blasSubstract(pos, r->position, oc);
    float l2oc = blasDot(oc, oc);
    float tca = blasDot(oc, r->direction);
    float l2hc = sqrRadius - l2oc + tca*tca;
    if(l2oc < sqrRadius) return tca + sqrt(l2hc);
    else
      if(tca < 0) return std::numeric_limits<float>::infinity();
      else return l2hc > 0 ?
          tca - sqrt(l2hc) : std::numeric_limits<float>::infinity();
  }
};
