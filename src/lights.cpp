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
    blasCopy(pos, lpos);
  }

};

class AreaLight : public Light {
public:

  int height;
  int width;
  int samples;
  double *samplesPos;
  double dir[3];

  AreaLight() {

    int h = 10;
    int w = 10;
    int gridHeight = 5;
    int gridWidth = 5;
    int sampleDensity = 1;
    double posx = 0; double posy = 25; double posz = 0;
    double colorR = 1; double colorG = 1; double colorB = 1;

    int hcenter;
    int wcenter;
    int hdelta;
    int wdelta;

    hcenter = h/2;
    wcenter = w/2;
    hdelta = h/gridHeight;
    wdelta = w/gridWidth;
    samples = gridHeight*gridWidth*sampleDensity;

    dir[0] = 0.0; dir[1] = -1.0; dir[2] = 0.0;
    pos[0] = posx; pos[1] = posy; pos[2] = posz;

    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = 10/samples;
    damping = 1;

    samplePos = new double[samples*3];

    int k = 0;
    for(int y = 0; y < gridHeight; y++) {
      for(int x = 0; x < gridWidth; x++) {
        for(int i = 0; i < samples; i++) {
          samplePos[k] = - wcenter + (rand() / (double(RAND_MAX))) - 0.5 + x*wdelta;
          samplePos[k+1] = 0;
          samplePos[k+2] = - hcenter + (rand() / (double(RAND_MAX))) - 0.5 + y*hdelta;
          blasAdd(pos, samplePos+k, samplePos+k);
          k+=3;
        }
      }
    }

  }

  AreaLight(
    int h, int w, int gridHeight, int gridWidth, int sampleDensity_in,
    double posx, double posy, double posz,
    double nrmx, double nrmy, double nrmz,
    double colorR, double colorG, double colorB,
    double i, double d) {

    int hcenter;
    int wcenter;
    int hdelta;
    int wdelta;
    int sampleDensity;
    double mat[9];

    hcenter = h/2;
    wcenter = w/2;
    hdelta = h/gridHeight;
    wdelta = w/gridWidth;
    sampleDensity = sampleDensity_in;
    samples = gridHeight*gridWidth*sampleDensity;

    dir[0] = nrmx; dir[1] = nrmy; dir[2] = nrmz;
    pos[0] = posx; pos[1] = posy; pos[2] = posz;

    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    intensity = i/samples;
    damping = d;

    samplePos = new double[samples*3];
//    blasBuildRotMat(pos, dir, mat);

    int k = 0;
    for(int y = 0; y < gridHeight; y++) {
      for(int x = 0; x < gridWidth; x++) {
        for(int z = 0; z < sampleDensity; z++) {
          samplePos[k] = - wcenter + (rand() / (double(RAND_MAX))) - 0.5 + x*wdelta;
          samplePos[k+1] = 0;
          samplePos[k+2] = - hcenter + (rand() / (double(RAND_MAX))) - 0.5 + y*hdelta;
          blasAdd(pos, samplePos+k, samplePos+k);
//          blasVecMatrixProd(samplePos+k, mat, samplePos+k);
          k+=3;
        }
      }
    }
  }

  ~AreaLight() {delete [] samplePos;}

  int getSamples() {
    return samples;
  }

  void getPosI(int i, double *lpos) {
    blasCopy(samplePos+i*3, lpos);
  }
};

