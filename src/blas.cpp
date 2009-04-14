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

void blasBuildRotMat(
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

void blasBuildRotMat(
    const double *pos,
    const double *dir,
    double *m) {

      double nV[3];     // Normalized direction vector.
      double nV_YZ[3];  // LookAt YZ plane projection direction vector.
      double nV_YZNrm;
      double nV_XZ[3];  // LookAt YZ plane projection direction vector.
      double nV_XZNrm;
      double nV_XY[3];  // LookAt YZ plane projection direction vector.
      double nV_XYNrm;

      // Canonic Vectors
      double X[3] = {0.0}; X[0] = 1.0;
      double Y[3] = {0.0}; Y[1] = 1.0;
      double Z[3] = {0.0}; Z[2] = 1.0;

      blasSubstract(pos, dir, nV);
      blasNormalize(nV);
      blasInvert(nV, nV);

      blasCopy(nV, nV_YZ);  nV_YZ[0] = 0.0;
      blasCopy(nV, nV_XZ);  nV_XZ[1] = 0.0;
      blasCopy(nV, nV_XY);  nV_XY[2] = 0.0;

      nV_YZNrm = blasNrm2(nV_YZ);
      nV_XZNrm = blasNrm2(nV_XZ);
      nV_XYNrm = blasNrm2(nV_XY);

      double cosA, sinA;
      cosA = blasDot(nV_YZ, Z) * nV_YZNrm;
      sinA = nV_YZ[1]/nV_YZNrm;

      double cosB, sinB;
      cosB = blasDot(nV_XZ, Z) * nV_XZNrm;
      sinB = nV_XZ[0]/nV_XZNrm;

    //  double cosC, sinC;
    //  cosC = blasDot(nV_XZ, Z) * nV_XZNrm;
    //  sinC = nV_XZ[0]/nV_XZNrm;

      blasBuildRotMat(cosA, -sinA, cosB, sinB, 1, 0, m);
//      std::cout <<
//      rayTransformationMat[0] << ", " <<
//      rayTransformationMat[1] << ", " <<
//      rayTransformationMat[2] << ";" << '\n' <<
//      rayTransformationMat[3] << ", " <<
//      rayTransformationMat[4] << ", " <<
//      rayTransformationMat[5] << ";" << '\n' <<
//      rayTransformationMat[6] << ", " <<
//      rayTransformationMat[7] << ", " <<
//      rayTransformationMat[8] << '\n';

}

void blasVecMatrixProd(const double *x, const double *m, double *mx) {
  mx[0] = x[0]*m[0] + x[1]*m[3] + x[2]*m[6];
  mx[1] = x[0]*m[1] + x[1]*m[4] + x[2]*m[7];
  mx[2] = x[0]*m[2] + x[1]*m[5] + x[2]*m[8];
}

float blasfastInvSqrt(register float x) {
  float xhalf = 0.5f*x;
  union
  {
    float x;
    int i;
  } u;
  u.x = x;
  u.i = 0x5f3759df - (u.i >> 1);
  x = u.x * (1.5f - xhalf * u.x * u.x);
  return x;
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

void blasNormalize(double *x) {
  register double nrm = sqrt(blasDot(x, x));
  x[0] = x[0]/nrm; x[1] = x[1]/nrm; x[2] = x[2]/nrm;
}

void blasFastNormalize(double *x) {
  register float nrm;
  nrm = blasfastInvSqrt((x[0] * x[0]) + (x[1] * x[1]) + (x[2] * x[2]));
  x[0] = x[0]*nrm; x[1] = x[1]*nrm; x[2] = x[2]*nrm;
}

double blasNrm2(const double *x) {
  return sqrt(blasDot(x, x));
}

int testBlas() {
  double m[9] = {4, 5, 6,         7, 8, 9,          10, 11, 12};
  double x[3] = {2, 4, 6};
  double y[3] = {2.0, 4.0, 8.0};
  double z[3] = {0.0, 0.0, 0.0};

  std::cout << "\nRunning Blas Test Suite...\n\n";
  std::cout << "X = [" << x[0] <<','<< x[1] <<','<< x[2] << "]\n";
  std::cout << "Y = [" << y[0] <<','<< y[1] <<','<< y[2] << "]\n";
  std::cout << "Z = [" << z[0] <<','<< z[1] <<','<< z[2] << "]\n\n";

  std::cout << "X dot Y: " << blasDot(x, y) << std::endl;

  blasScale(x, 2, z);
  std::cout << "2*X->Z = : [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;

  blasCross(x, y, z);
  std::cout << "X cross Y -> Z = [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;

  blasCopy(x, z);
  blasNormalize(z);
  std::cout << "Normalized X -> Z = [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;
  std::cout << "Norm-2: " << blasNrm2(z) << std::endl;

  blasCopy(x, z);
  blasFastNormalize(z);
  std::cout << "Fast Normalized X -> Z = [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;
  std::cout << "Norm-2: " << blasNrm2(z) << std::endl;

  blasBuildRotMat(0, 1, 1, 0, 1, 0, m);
  blasVecMatrixProd(x, m, z);
  std::cout << "X*M->Z = [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;

  return 0;
}

