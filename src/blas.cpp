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

void blasRotVec(const float *v,
    const float c, const float s, float *m) {

  float u = 1 - c;

  m[0] = v[0]*v[0]*u + c;
  m[1] = v[1]*v[0]*u - v[2]*s;
  m[2] = v[2]*v[0]*u + v[1]*s;
  m[3] = v[0]*v[1]*u + v[2]*s;
  m[4] = v[1]*v[1]*u + c;
  m[5] = v[2]*v[1]*u - v[0]*s;
  m[6] = v[0]*v[1]*u - v[1]*s;
  m[7] = v[1]*v[0]*u + v[0]*s;
  m[8] = v[2]*v[2]*u + c;
}

void blasRotFromTo(float *from, float *to, float *mat) {

  float v[3], e, h, f;
  blasCross(from, to, v);
  e = blasDot(from, to);
  f = (e < 0) ? -e : e;
  if(f > (1.0 - EPS_0)) {
    float u[3], v[3], x[3];
    float c1, c2, c3;
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
    float hvx, hvz, hvxy, hvxz, hvyz;
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

void blasRotNV(const float *n, const float *up, float *m) {

  float tmp[3] = {0.0};
  float upProj = blasDot(n, up);
  blasScale(n, upProj, tmp);
  blasSubstract(up, tmp, tmp);

  if(fabs(tmp[0]) + fabs(tmp[1]) + fabs(tmp[2]) < EPS_0) {
    tmp[0] = -n[1]*n[0];
    tmp[1] = 1-n[1]*n[1];
    tmp[2] = -n[1]*n[2];
  }
  if(fabs(tmp[0]) + fabs(tmp[1]) + fabs(tmp[2]) < EPS_0) {
    tmp[0] = -n[2]*n[0];
    tmp[1] = -n[2]*n[1];
    tmp[2] = 1-n[2]*n[2];
  }
  blasNormalize(tmp);

  if(upProj < 0)
    blasInvert(tmp, tmp);

  float u[3], v[3];
  blasCross(n, tmp, u);
  if(fabs(u[0]) + fabs(u[1]) + fabs(u[2]) > 0)
    blasNormalize(u);
  blasCross(n, u, v);

  m[0] = u[0];
  m[1] = u[1];
  m[2] = u[2];

  m[3] = v[0];
  m[4] = v[1];
  m[5] = v[2];

  m[6] = n[0];
  m[7] = n[1];
  m[8] = n[2];

}

void blasMatMatProd(const float *a, const float *b, float *ab) {

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

void blasVecMatrixProd(const float *x, const float *m, float *mx) {
  float tmp[3];
  tmp[0] = x[0]*m[0] + x[1]*m[3] + x[2]*m[6];
  tmp[1] = x[0]*m[1] + x[1]*m[4] + x[2]*m[7];
  tmp[2] = x[0]*m[2] + x[1]*m[5] + x[2]*m[8];
  mx[0] = tmp[0]; mx[1] = tmp[1]; mx[2] = tmp[2];
}

void blasAdd(const float *x, const float *y, float *z) {
  float tmp[3];
  tmp[0] = x[0] + y[0];
  tmp[1] = x[1] + y[1];
  tmp[2] = x[2] + y[2];
  z[0] = tmp[0]; z[1] = tmp[1]; z[2] = tmp[2];
}

void blasSubstract(const float *x, const float *y, float *z) {
  float tmp[3];
  tmp[0] = x[0] - y[0];
  tmp[1] = x[1] - y[1];
  tmp[2] = x[2] - y[2];
  z[0] = tmp[0]; z[1] = tmp[1]; z[2] = tmp[2];
}
float blasDot(const float *x, const float *y) {
  return (x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
}

void blasCross(const float *x, const float *y, float *z) {
  float tmp[3];
  tmp[0] = x[1] * y[2] - x[2] * y[1];
  tmp[1] = x[2] * y[0] - x[0] * y[2];
  tmp[2] = x[0] * y[1] - x[1] * y[0];
  z[0] = tmp[0]; z[1] = tmp[1]; z[2] = tmp[2];
}

void blasCopy(const float *x, float *y) {
  y[0] = x[0];
  y[1] = x[1];
  y[2] = x[2];
}

void blasScale(const float *x, register const float k, float *kx) {
  kx[0] = k*x[0];
  kx[1] = k*x[1];
  kx[2] = k*x[2];
}

void blasInvert(const float *x, float *invx) {
  invx[0] = -x[0];
  invx[1] = -x[1];
  invx[2] = -x[2];
}

float blasFastBabSqrt(const float x) {
  union
  {
    int i;
    float x;
  } u;
  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22);
  u.x =       u.x + x/u.x;
  u.x = 0.25f*u.x + x/u.x;

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

void blasNormalize(float *x) {
  register float nrm = 1;
  nrm /= sqrtf(blasDot(x, x));
  x[0] *= nrm; x[1] *= nrm; x[2] *= nrm;
}

void blasFastNormalize(float *x) {
  register float nrm = 1;
  nrm /= blasFastBabSqrt((x[0] * x[0]) + (x[1] * x[1]) + (x[2] * x[2]));
  x[0] *= nrm; x[1] *= nrm; x[2] *= nrm;
}

void blasVeryFastNormalize(float *x) {
  register float nrm = blasFastInvSqrt((x[0] * x[0]) + (x[1] * x[1]) + (x[2] * x[2]));
  x[0] *= nrm; x[1] *= nrm; x[2] *= nrm;
}

float blasNrm2(const float *x) {
  return sqrt(blasDot(x, x));
}

