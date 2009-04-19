/*
 * File: blas.h
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

#ifndef BLAS_H_
#define BLAS_H_

#include <iostream>
#include <math.h>

#define EPS_0 0.000001
#define EPS_1 0.00001
#define EPS_2 0.0001
#define EPS_3 0.001

#define SQRT_MAGIC_F 0x5f3759df

void blasBuildRotMatDir(float *nV, float *mat);

void blasBuildRotMatAngles(
    const float cosAlpha, const float sinAlpha,
    const float cosBeta, const float sinBeta,
    const float cosGamma, const float sinGamma,
    float *m);

void blasBuildRotMatFromTo(float *from, float *to, float *mat);

void blasMatMatProd(const float *a, const float *b, float *ab);

void blasVecMatrixProd(const float *x, const float *m, float *mx);

void blasAdd(const float *x, const float *y, float *z);

void blasSubstract(const float *x, const float *y, float *z);

float blasDot(const float *x, const float *y);

void blasCross(const float *x, const float *y, float *z);

void blasCopy(const float *x, float *y);

void blasScale(const float *x, register const float k, float *kx);

void blasInvert(const float *x, float *invx);

float blasFastBabSqrt(const float x);

float blasFastInvSqrt(const float x);

void blasNormalize(float *x);

void blasFastNormalize(float *x);

void blasVeryFastNormalize(float *x);

float blasNrm2(const float *x);

#endif /* BLAS_H_ */
