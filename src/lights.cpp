/*
 * File: lights.cpp
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

#include "lights.h"

class Omnidirectional : public Light {
public:

  Omnidirectional() {
    pos[0] = 0.0; pos[1] = 10.0; pos[2] = 0.0;
    color[0] = color[1] = color[2] = 1.0;
    intensity = 1.0;
    damping = 1.0;
  }

  Omnidirectional(float posx, float posy, float posz,
                  float colorR, float colorG, float colorB,
                  float i, float d) {

    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = i;
    damping = d;
  }

  int getSamples() {
    return 1;
  }

  void getPosI(int i, float *lpos) {
    blasCopy(pos, lpos);
  }

};

class AreaLight : public Light {
public:

  float height;
  float width;
  int gridHeight;
  int gridWidth;
  int sampleDensity;
  int samples;
  float *samplesPos;
  float dir[3];

  void setSamples() {

    float hcenter;
    float wcenter;
    float hdelta;
    float wdelta;
    float mat[9];

    hcenter = height/2;
    wcenter = width/2;
    hdelta = height/gridHeight;
    wdelta = width/gridWidth;
    samples = gridHeight*gridWidth*sampleDensity;
    intensity /= samples;

    blasNormalize(dir);
    blasBuildRotMatDir(dir, mat);

    samplePos = new float[samples*3];
    float posNrm;

    int k = 0;
    for(int y = 0; y < gridHeight; y++) {
      for(int x = 0; x < gridWidth; x++) {
        for(int z = 0; z < sampleDensity; z++) {
          samplePos[k] = - wcenter + (rand() / (float(RAND_MAX))) + wdelta/2 + x*wdelta;
          samplePos[k+1] = - hcenter + (rand() / (float(RAND_MAX))) + hdelta/2 + y*hdelta;
          samplePos[k+2] = 0;
          posNrm = blasNrm2(samplePos+k);
          blasVecMatrixProd(samplePos+k, mat, samplePos+k);
          blasNormalize(samplePos+k);
          blasScale(samplePos+k, posNrm, samplePos+k);
          blasAdd(pos, samplePos+k, samplePos+k);
          k+=3;
        }
      }
    }
  }

  AreaLight() {

    height = 5;
    width = 5;
    sampleDensity = 5;
    gridHeight = 5;
    gridWidth = 5;

    dir[0] = 0.0; dir[1] = -1.0; dir[2] = 0.0;
    pos[0] = 0.0; pos[1] = 25; pos[2] = 0.0;
    color[0] = 1; color[1] = 1; color[2] = 1;

    intensity = 1;
    damping = 1;

  }

  AreaLight(
    float h, float w, int gridHeight_in, int gridWidth_in, int sampleDensity_in,
    float posx, float posy, float posz,
    float dirx, float diry, float dirz,
    float colorR, float colorG, float colorB,
    float i, float d) {

    height = h;
    width = w;
    gridHeight = gridHeight_in;
    gridWidth = gridWidth_in;
    sampleDensity = sampleDensity_in;

    dir[0] = dirx; dir[1] = diry; dir[2] = dirz;
    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;

    intensity = i;
    damping = d;

    setSamples();
  }

  ~AreaLight() {delete [] samplePos;}

  int getSamples() {
    return samples;
  }

  void getPosI(int i, float *lpos) {
    blasCopy(samplePos+i*3, lpos);
  }
};

