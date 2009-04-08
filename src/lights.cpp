/*
 * File: lights.cpp
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

#include "lights.h"
#include "blas.h"

class Unidirectional : public Light {
public:
  double dir[3];

  Unidirectional() {
    pos[0] = pos[1] = 0.0, pos[2] = 10.0;
    dir[0] = dir[1] = 0.0; dir[2] = -1;
    color[0] = color[1] = color[2] = 1.0;
    intensity = 1.0;
    damping = 1.0;
  }

  Unidirectional(double posx, double posy, double posz,
                 double dirx, double diry, double dirz,
                 double colorR, double colorG, double colorB,
                 double i, double d) {

    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    dir[0] = dirx; dir[1] = diry; dir[2] = dirz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = i;
    damping = d;
  }
};

class Omnidirectional : public Light {
public:

  Omnidirectional() {
    pos[0] = 0.0; pos[1] = 10.0; pos[2] = 0.0;
    color[0] = color[1] = color[2] = 1.0;
    intensity = 1.0;
    damping = 1.0;
  }

  Omnidirectional(double posx, double posy, double posz,
                  double colorR, double colorG, double colorB,
                  double i, double d) {

    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = i;
    damping = d;
  }
};

class AreaLight : public Light {
public:

  AreaLight() {
    pos[0] = 0.0; pos[1] = 10.0; pos[2] = 0.0;
    color[0] = color[1] = color[2] = 1.0;
    intensity = 1.0;
    damping = 1.0;
  }

  AreaLight(double posx, double posy, double posz,
                  double colorR, double colorG, double colorB,
                  double i, double d) {

    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = i;
    damping = d;
  }
};
