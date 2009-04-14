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

void blasBuildRotMat(
    const double cosAlpha, const double sinAlpha,
    const double cosBeta, const double sinBeta,
    const double cosGamma, const double sinGamma,
    double *m);

void blasVecMatrixProd(const double *x, const double *m, double *mx);

float blasfastInvSqrt(float x);

void blasAdd(const double *x, const double *y, double *z);

void blasSubstract(const double *x, const double *y, double *z);

double blasDot(const double *x, const double *y);

void blasCross(const double *x, const double *y, double *z);

void blasCopy(const double *x, double *y);

void blasScale(const double *x, register const double k, double *kx);

void blasInvert(const double *x, double *invx);

void blasNormalize(double *x);

void blasFastNormalize(double *x);

double blasNrm2(const double *x);

int testBlas();

#endif /* BLAS_H_ */
