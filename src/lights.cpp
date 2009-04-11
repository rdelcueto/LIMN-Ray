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

  int getSamples() {
    return 1;
  }

  void getPosI(int i, double *lpos) {
    blas3Dcopy(pos, lpos);
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

  int getSamples() {
    return 1;
  }

  void getPosI(int i, double *lpos) {
    blas3Dcopy(pos, lpos);
  }

};

class AreaLight : public Light {
public:

  int height;
  int width;
  int hcenter;
  int wcenter;
  int gridHSize;
  int gridWSize;
  int hdelta;
  int wdelta;
  int sampleDensity;
  double dir[3];

  AreaLight() {
    height = 4;
    width = 4;
    hcenter = height/2;
    wcenter = width/2;
    gridHSize = 4;
    gridWSize = 4;
    hdelta = height/gridHSize;
    wdelta = width/gridWSize;
    sampleDensity = 1;
    pos[0] = 0.0; pos[1] = 25.0; pos[2] = 0.0;
    dir[0] = 0.0; dir[1] = -1.0; dir[2] = 0.0;
    color[0] = color[1] = color[2] = 1.0;
    intensity = 5;
    damping = 1.0;
  }

  AreaLight(
    int h, int w, int gridHeight, int gridWidth, int sampleDensity_in,
    double posx, double posy, double posz,
    double colorR, double colorG, double colorB,
    double i, double d) {

    height = h;
    width = w;
    hcenter = h/2;
    wcenter = w/2;
    gridHSize = gridHeight;
    gridWSize = gridWidth;
    hdelta = height/gridHSize;
    wdelta = width/gridWSize;
    sampleDensity = sampleDensity_in;

    dir[0] = 0.0; dir[1] = -1.0; dir[2] = 0.0;
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = i;
    damping = d;
  }

  int getSamples() {
    return sampleDensity*gridHSize*gridWSize;
  }

  void getPosI(int i, double *lpos) {
    lpos[0] = (i%width)*wdelta*(rand() / (double(RAND_MAX)*2)) - wcenter;
    lpos[1] = 0;
    lpos[2] = (i/width)*hdelta*(rand() / (double(RAND_MAX)*2)) - hcenter;
    blas3dAddXY(pos, lpos, lpos);
  }
};
