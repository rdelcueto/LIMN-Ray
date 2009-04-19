/*
 * File: blas.cpp
 * Author: Rodrigo González del Cueto
 * Implementation of Basic Linear Algebra Subprograms to map some of CUBLAS
 * methods.
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

#include "blas.h"

void blasBuildRotMatDir(double *v, double *mat) {

  double nV_YZ[3];  // LookAt YZ plane projection direction vector.
  double nV_XZ[3];  // LookAt YZ plane projection direction vector.
  double nV_XY[3];  // LookAt YZ plane projection direction vector.

  double matA[9], matB[9];

  // Canonic Vectors
  double X[3] = {0.0}; X[0] = 1.0;
  double Y[3] = {0.0}; Y[1] = 1.0;
  double Z[3] = {0.0}; Z[2] = 1.0;

  blasCopy(v, nV_YZ);  nV_YZ[0] = 0; nV_YZ[2] += EPS_0;
  blasCopy(v, nV_XZ);  nV_XZ[1] = 0; nV_XZ[2] += EPS_0;
  blasCopy(v, nV_XY);  nV_XY[2] = 0;

  blasNormalize(nV_YZ);
  blasBuildRotMatFromTo(nV_YZ, Z, matA);

  blasNormalize(nV_XZ);
  blasBuildRotMatFromTo(nV_XZ, Z, matB);

  blasMatMatProd(matA, matB, mat);

//  std::cout <<
//  mat[0] << ", " <<
//  mat[1] << ", " <<
//  mat[2] << ";" << '\n' <<
//  mat[3] << ", " <<
//  mat[4] << ", " <<
//  mat[5] << ";" << '\n' <<
//  mat[6] << ", " <<
//  mat[7] << ", " <<
//  mat[8] << '\n';

}

void blasBuildRotMatAngles(
    const double cosAlpha, const double sinAlpha,
    const double cosBeta, const double sinBeta,
    const double cosGamma, const double sinGamma,
    double *m) {

    m[0] = cosGamma*cosBeta;
    m[1] = sinGamma*cosBeta;
    m[2] = -sinBeta;

    m[3] = -sinGamma*cosAlpha + cosGamma*sinBeta*sinAlpha;
    m[4] = cosGamma*cosAlpha + sinGamma*sinBeta*sinAlpha;
    m[5] = cosBeta*sinAlpha;

    m[6] = sinGamma*sinAlpha + cosGamma*sinBeta*cosAlpha;
    m[7] = -cosGamma*sinAlpha + sinGamma*sinBeta*cosAlpha;
    m[8] = cosBeta*cosAlpha;

}

void blasBuildRotMatFromTo(double *from, double *to, double *mat) {

  double v[3], e, h, f;
  blasCross(from, to, v);
  e = blasDot(from, to);
  f = (e < 0) ? -e : e;
  if(f > (1.0 - EPS_0)) {
    double u[3], v[3], x[3];
    double c1, c2, c3;
    int i, j;

    x[0] = (from[0] > 0.0)? from[0] : -from[0];
    x[1] = (from[1] > 0.0)? from[1] : -from[1];
    x[2] = (from[2] > 0.0)? from[2] : -from[2];

    if(x[0] < x[1]) {
      if(x[0] < x[2]) {
        x[0] = 1.0; x[1] = x[2] = 0.0;
      }
      else {
        x[2] = 1.0; x[0] = x[1] = 0.0;
      }
    }
    else {
      if(x[1] < x[2]) {
        x[1] = 1.0; x[0] = x[2] = 0.0;
      }
      else {
        x[2] = 1.0; x[0] = x[1] = 0.0;
      }
    }

    u[0] = x[0] - from[0]; u[1] = x[1] - from[1]; u[2] = x[2] - from[2];
    v[0] = x[0] - to[0];   v[1] = x[1] - to[1];   v[2] = x[2] - to[2];

    c1 = 2.0/blasDot(u, u); c2 = 2.0/blasDot(v, v); c3 = c1*c2*blasDot(u, v);

    for(i = 0; i < 3; i++) {
      for(j = 0; j < 3; j++) {
        mat[i*3+j] = - c1 *u[i]*u[j] - c2*v[i]*v[j] + c3*v[i]*u[j];
      }
      mat[i*3+i] += 1.0;
    }
  }

  else {
    double hvx, hvz, hvxy, hvxz, hvyz;
    h = 1.0/(1.0 + e);
    hvx = h * v[0];
    hvz = h * v[2];
    hvxy = hvx * v[1];
    hvxz = hvx * v[2];
    hvyz = hvz * v[1];
    mat[0] = e + hvx * v[0];
    mat[1] = hvxy - v[2];
    mat[2] = hvxz + v[1];

    mat[3] = hvxy + v[2];
    mat[4] = e + h * v[1] * v[1];
    mat[5] = hvyz - v[0];

    mat[6] = hvxz - v[1];
    mat[7] = hvyz + v[0];
    mat[8] = e + hvz * v[2];
  }
}

void blasMatMatProd(const double *a, const double *b, double *ab) {

    ab[0] = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
    ab[1] = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
    ab[2] = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];

    ab[3] = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
    ab[4] = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
    ab[5] = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];

    ab[6] = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
    ab[7] = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
    ab[8] = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];

}

void blasVecMatrixProd(const double *x, const double *m, double *mx) {
  mx[0] = x[0]*m[0] + x[1]*m[3] + x[2]*m[6];
  mx[1] = x[0]*m[1] + x[1]*m[4] + x[2]*m[7];
  mx[2] = x[0]*m[2] + x[1]*m[5] + x[2]*m[8];
}

void blasAdd(const double *x, const double *y, double *z) {
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  z[2] = x[2] + y[2];
}

void blasSubstract(const double *x, const double *y, double *z) {
  z[0] = x[0] - y[0];
  z[1] = x[1] - y[1];
  z[2] = x[2] - y[2];
}
double blasDot(const double *x, const double *y) {
  return (x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
}

void blasCross(const double *x, const double *y, double *z) {
  z[0] = x[1] * y[2] - x[2] * y[1];
  z[1] = x[2] * y[0] - x[0] * y[2];
  z[2] = x[0] * y[1] - x[1] * y[0];
}

void blasCopy(const double *x, double *y) {
  y[0] = x[0];
  y[1] = x[1];
  y[2] = x[2];
}

void blasScale(const double *x, register const double k, double *kx) {
  kx[0] = k*x[0];
  kx[1] = k*x[1];
  kx[2] = k*x[2];
}

void blasInvert(const double *x, double *invx) {
  invx[0] = -x[0];
  invx[1] = -x[1];
  invx[2] = -x[2];
}

double blasFastBabSqrt(const double x) {
  union
  {
    long i;
    double x;
  } u;
  u.x = x;
  u.i = (((long)1)<<61) + (u.i >> 1) - (((long)1)<<51);
  u.x =       u.x + x/u.x;
  u.x = 0.25F*u.x + x/u.x;

  return u.x;
}

float blasFastInvSqrt(const float x) {
  const float xhalf = 0.5f*x;
  union
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = SQRT_MAGIC_F - (u.i >> 1);
  return u.x*(1.5f - xhalf*u.x*u.x);
}

void blasNormalize(double *x) {
  register double nrm = 1;
  nrm /= sqrt(blasDot(x, x));
  x[0] *= nrm; x[1] *= nrm; x[2] *= nrm;
}

void blasFastNormalize(double *x) {
  register double nrm = 1;
  nrm /= blasFastBabSqrt((x[0] * x[0]) + (x[1] * x[1]) + (x[2] * x[2]));
  x[0] *= nrm; x[1] *= nrm; x[2] *= nrm;
}

void blasVeryFastNormalize(double *x) {
  register double nrm = blasFastInvSqrt((x[0] * x[0]) + (x[1] * x[1]) + (x[2] * x[2]));
  x[0] *= nrm; x[1] *= nrm; x[2] *= nrm;
}

double blasNrm2(const double *x) {
  return sqrt(blasDot(x, x));
}

