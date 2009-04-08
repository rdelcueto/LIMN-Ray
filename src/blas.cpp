/*
 * File: blas.cpp
 * Author: Rodrigo González del Cueto
 * Implementation of Basic Linear Algebra Subprograms to map some of CUBLAS
 * methods.
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

#include <iostream>
#include <math.h>
#include "blas.h"

int blasIdamax(int n, const double *x, int incx) {

  if(n <= 0 || incx <= 0) return 0;

  int i;
  int maxId = 0;
  double tmp;
  double max = x[0];

  for(i = 1; i < n; i++) {
    tmp = fabs(x[i*incx]);
    if(max < tmp) {
      max = tmp;
      maxId = i;
    }
  }
  return maxId + 1;
}

int blasIdamin(int n, const double *x, int incx) {

  if(n <= 0 || incx <= 0) return 0;

  int i;
  int minId = 0;
  double tmp;
  double min = x[0];

  for(i = 1; i < n; i++) {
    tmp = fabs(x[i*incx]);
    if(min > tmp) {
      min = tmp;
      minId = i;
    }
  }
  return minId + 1;
}

double blasDasum(int n, const double *x, int incx) {

  if(n <= 0 || incx <= 0) return 0;

  int i;
  double sum = 0;

  for(i = 0; i < n; i++)
    sum += fabs(x[i*incx]);

  return sum;
}

void blasDaxpy(int n, double alpha,
               const double *x, int incx,
               double *y, int incy) {

  if(n <= 0) return;

  int i;

  for(i=0; i < n; i++)
    y[i * incy] += alpha * x[i * incx];

}

void blasDcopy(int n, const double *x, int incx,
               double *y, int incy) {

  if(n <= 0) return;
  int i;

  for(i=0; i < n; i++)
    y[i * incy] = x[i * incx];

}

void blas3Dcopy(const double *x, double *y) {

  y[0] = x[0];
  y[1] = x[1];
  y[2] = x[2];

}

double blas3dDot(const double *x, const double *y) {
  return (x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
}

double blasDdot(int n, const double *x, int incx,
               const double *y, int incy) {

  if(n <= 0) return 0;

  register double sum = 0;
  for(register int i = 0; i < n; i++)
    sum  += y[i * incy] * x[i * incx];
  return sum;
}

double blasDnrm2(int n, const double *x, int incx) {

  if(n <= 0 || incx <=0) return 0;
  else return sqrt(blasDdot(n, x, 1, x, 1));

}

void blasDrot(int n, double *x, int incx, double *y, int incy,
               double dc, double ds) {

  int i;

  for(i = 0; i < n; i++) {
    x[i * incx] = dc * x[i * incx] + ds * y[i * incy];
    y[i * incy] = -ds * x[i * incx] + dc * y[i * incy];
  }

}

void blasDscal(int n, double alpha,
               double *x, int incx) {

  if(n <= 0) return;

  int i;

  for(i=0; i < n; i++)
    x[i * incx] *= alpha;

}

void blasDswap(int n,
               double *x, int incx,
               double *y, int incy) {

  if(n <= 0) return;

  int i;
  double tmp;

  for(i=0; i < n; i++) {
    tmp = y[i * incy];
    y[i * incy] = x[i * incx];
    x[i * incx] = tmp;
  }

}

void blas3DNormalize(double *x) {
  double nrm = sqrt(blas3dDot(x, x));
  x[0] /= nrm; x[1] /= nrm; x[2] /= nrm;
}

void blasNormalize(int n, double *x, int incx) {
  double nrm = blasDnrm2(n, x, incx);
  blasDscal(n, 1.0/nrm, x, incx);
}

void blas3dAddXY(const double *x,
                 const double *y,
                 double *z) {
  z[0] = x[0] + y[0];
  z[1] = x[1] + y[1];
  z[2] = x[2] + y[2];
}

void blasAddXY(int n,
                const double *x,
                const double *y,
                double *z) {
  for(register int i = 0; i < n; i++)
    z[i] = x[i] + y[i];
}

void blas3dSubstractXY(const double *x,
                       const double *y,
                       double *z) {
  z[0] = x[0] - y[0];
  z[1] = x[1] - y[1];
  z[2] = x[2] - y[2];
}

void blasSubstractXY(int n, const double *x,
                       const double *y,
                       double *z) {
  for(register int i = 0; i < n; i++)
   z[i] = x[i] - y[i];
}

void blas3DInvert(const double *x, double *y) {
    y[0] = -x[0];
    y[1] = -x[1];
    y[2] = -x[2];
}

void blasInvert(int n, const double *x, double *y) {
  for(register int i = 0; i < n; i++)
    y[i] = -x[i];
}

void blasCrossProd(const double *x, int incx,
                   const double *y, int incy,
                   double *z, int incz) {

  z[0] = x[1] * y[2] - x[2] * y[1];
  z[1] = x[2] * y[0] - x[0] * y[2];
  z[2] = x[0] * y[1] - x[1] * y[0];

}

int testBlas() {

  double x[3] = {1.0, 0.0, 0.0};
  double y[3] = {1.0, 1.0, 0.0};
  double z[3] = {0.0, 0.0, 0.0};

  std::cout << "\nRunning Blas Test Suite...\n\n";
  std::cout << "X = [" << x[0] <<','<< x[1] <<','<< x[2] << "]\n";
  std::cout << "Y = [" << y[0] <<','<< y[1] <<','<< y[2] << "]\n";
  std::cout << "Z = [" << z[0] <<','<< z[1] <<','<< z[2] << "]\n\n";

  std::cout << "MaxId X: " << blasIdamax(3, x, 1) << std::endl;
  std::cout << "MaxId Y: " << blasIdamax(3, y, 1) << std::endl;

  std::cout << "MinId X: " << blasIdamin(3, x, 1) << std::endl;
  std::cout << "MinId Y: " << blasIdamin(3, y, 1) << std::endl;

  std::cout << "AbsSum X: " << blasDasum(3, x, 1) << std::endl;
  std::cout << "AbsSum Y: " << blasDasum(3, y, 1) << std::endl;

  blasDaxpy(3, 0.0, x, 1, x, 1);
  std::cout << "0*X+X -> X: [" << x[0] << ',' << x[1] << ',' << x[2] << ']' << std::endl;
  blasDaxpy(3, 2.0, x, 1, y, 1);
  std::cout << "2*X + Y -> Y: [" << y[0] << ',' << y[1] << ',' << y[2] << ']' << std::endl;
  blasDcopy(3, x, 1, z, 1);
  std::cout << "X->Z: [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;

  std::cout << "X dot Y: " << blasDdot(3, x, 1, y, 1) << std::endl;

  std::cout << "Norm-L2(X): " << blasDnrm2(3, x, 1) << std::endl;
  std::cout << "Norm-L2(Y): " << blasDnrm2(3, y, 1) << std::endl;

  std::cout << "Z: [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;
  blasDscal(3, 2, z, 1);
  std::cout << "2*Z: [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;

  blasDswap(3, x, 1, y, 1);
  std::cout << "Y->X = [" << x[0] << ',' << x[1] << ',' << x[2] << ']' << std::endl;
  std::cout << "X->Y = [" << y[0] << ',' << y[1] << ',' << y[2] << ']' << std::endl;

  blasCrossProd(x, 1, y, 1, z, 1);
  std::cout << "X cross Y -> Z = [" << z[0] << ',' << z[1] << ',' << z[2] << ']' << std::endl;
  return 0;
}
