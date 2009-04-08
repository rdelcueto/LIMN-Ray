/*
 * File: blas.h
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

#ifndef BLAS_H_
#define BLAS_H_

#include <iostream>
#include <math.h>

/* cublasIdamax()
 * Finds the smallest index of the maximum magnitude element of the
 * double-vector x.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 */


int blasIdamax(int n, const double *x, int incx);

/* cublasIdamin()
 * Finds the smallest index of the minimum magnitude element of the
 * double-vector x.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 */

int blasIdamin(int n, const double *x, int incx);

/* cublasDasum()
 * Computes the sum of the absolute values of the elements of double precision of the double-vector x.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 */

double blasDasum(int n, const double *x, int incx);

/* blasDaxpy()
 * Multiplies double-precision vector x by double-presicion vector scalar alpha
 * and adds the result to double-precision vector y; That is, it overwrites
 * double-precision y with double-precision alpha * x + y.
 *
 * alpha Double-precision scalar multiplier.
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 * y Double-precision vector with n elements.
 * incy Storage spacing between elements of y.
 */

void blasDaxpy(int n, double alpha,
               const double *x, int incx,
               double *y, int incy);

/* blasDcopy()
 * Copies the double precision vector x to the double-precision vector y.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 * y Double-precision vector with n elements.
 * incy Storage spacing between elements of y.
 */

void blasDcopy(int n, const double *x, int incx,
               double *y, int incy);

void blas3Dcopy(const double *x, double *y);

/* blasFast3dDot()
 * Computes the dot product of two double-precision 3D vectors.
 */

double blas3dDot(const double *x, const double *y);

/* blasDdot()
 * Computes the dot product of two double-precision vectors.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 * y Double-precision vector with n elements.
 * incy Storage spacing between elements of y.
 */

double blasDdot(int n, const double *x, int incx,
               const double *y, int incy);

/* blasDnrm2()
 * Computes the Euclidean norm of the double-precision n-vector x.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 */

double blasDnrm2(int n, const double *x, int incx);

/* cublasDrot()
 * Multiplies a 2x2 matrix [dc,ds;-ds,dc] with 2 x n matrix [x^t; y^t].
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 * y Double-precision vector with n elements.
 * incy Storage spacing between elements of y.
 */

void blasDrot(int n, double *x, int incx, double *y, int incy,
               double dc, double ds);

/* blasDscal()
 * Multiplies double-precision vector x by double-presicion vector scalar alpha
 *
 * alpha Double-precision scalar multiplier.
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 */

void blasDscal(int n, double alpha,
               double *x, int incx);

/* blasDswap()
 * Interchanges double-precision vector x with double-presicion vector y.
 *
 * n Number of elements in input vector.
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 * y Double-precision vector with n elements.
 * incy Storage spacing between elements of y.
 */

void blasDswap(int n,
               double *x, int incx,
               double *y, int incy);

void blas3DNormalize(double *x);

void blasNormalize(int n, double *x, int incx);

void blas3dAddXY(const double *x,
                 const double *y,
                 double *z);

void blasAddXY(int n,
                const double *x,
                const double *y,
                double *z);

void blasSubstractXY(int n, const double *x,
                       const double *y,
                       double *z);

void blas3dSubstractXY(const double *x,
                       const double *y,
                       double *z);

void blasInvert(int n, const double *x, double *y);
void blas3DInvert(const double *x, double *y);

/* blasCrossProd()
 * Computes the cross product of a double-precision vector x of size 3 with a
 * double-presicion vector y of size 3.
 *
 * x Double-precision vector with n elements.
 * incx Storage spacing between elements of x.
 * y Double-precision vector with n elements.
 * incy Storage spacing between elements of y.
 */

void blasCrossProd(const double *x, int incx,
                   const double *y, int incy,
                   double *z, int incz);
/*
 * Test Suite
 */

int testBlas();

#endif /* BLAS_H_ */
