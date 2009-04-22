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

/**
 * @class Omnidirectional
 *
 * @brief This class describes an omnidirectional/point light source.
 *
 * An omnidirectional light simply represents a point in space, that throws rays
 * in every direction.
 */
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

/**
 * @class AreaLight
 *
 * @brief This class describes an Area source light.
 *
 * An arealight is supposed to model a surface that emits light. For this
 * implementation we just model rectangles. The idea is to simulate what an
 * infinitely dense grid of point lights in all of the rectangle area would
 * iluminate. For this we define an area light by the rectangle's dimensions
 * and the grid size for the subsamples of the light source. Each grid square
 * the number of samples specified by AreaLight::sampleDensity.
 * The higher resolution of the grid and the sample density, the better the
 * result of the approximation.
 *
 * In order to prevent the aliasing in lights and shadows cause by the sampling
 * grid, we randomize the position of every sample across the area of their
 * containing grid.
 */

class AreaLight : public Light {
public:

  /** The area light rectangle's height dimension.*/
  float height;
  /** The area light rectangle's width dimension.*/
  float width;
  /** The area light grid's height resolution.*/
  int gridHeight;
  /** The area light grid's width resolution.*/
  int gridWidth;
  /** The area light's sample number per square in it's sampling grid*/
  int sampleDensity;
  /** The total number of samples in the area light.
   * #samples = (#gridHeight x #gridWidth * #sampleDensity)*/
  int samples;
  /** An array of 3D vectors containing the vector position of each sample.*/
  float *samplePos;
  /** A 3D vector representing the direction normal to the area lights plane.*/
  float dir[3];

  /**
   * This function constructs the position of each sample and sets the remaining
   * variables that are dependant of the first variables of the constructor.
   */
  void setSamples() {

    float hcenter;
    float wcenter;
    float hdelta;
    float wdelta;
    float mat[9];

    hdelta = height/(1.0 + gridHeight);
    wdelta = width/(1.0 + gridWidth);
    hcenter = -height/2.0 + hdelta/2.0;
    wcenter = -width/2.0 + wdelta/2.0;

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
          samplePos[k] = wcenter + (rand() / (float(RAND_MAX)))*wdelta;
          samplePos[k+1] = hcenter + (rand() / (float(RAND_MAX)))*hdelta;
          samplePos[k+2] = 1;
          posNrm = blasNrm2(samplePos+k);
          blasVecMatrixProd(samplePos+k, mat, samplePos+k);
          blasNormalize(samplePos+k);
          blasScale(samplePos+k, posNrm, samplePos+k);
          blasAdd(pos, samplePos+k, samplePos+k);
          k+=3;
        }
        wcenter += wdelta;
      }
      wcenter = -width/2.0 + wdelta/2.0;
      hcenter += hdelta;
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

    pos[0] = posx; pos[1] = posy; pos[2] = posz;
    color[0] = colorR; color[1] = colorG; color[2] = colorB;
    dir[0] = dirx; dir[1] = diry; dir[2] = dirz;
    blasNormalize(dir);

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

