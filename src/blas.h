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
#define EPS_3 0.005

/// Quake 3 Magic constant for calculating the fast invSqrt().
#define SQRT_MAGIC_F 0x5f3759df


/**
 * This function creates a rotation matrix from an arbitrary vector to the Z
 * canonic vector.
 *
 * @param nV Normalized arbitrary direction vector.
 * @param mat 3x3 rotation matrix result.
 */
void blasBuildRotMatDir(float *nV, float *mat);

/**
 * This function creates a rotation matrix for the given sines and cosines
 * in each axis.
 *
 * @param cosAlpha The cosine of the alpha angle.
 * @param sinAlpha The sine of the alpha angle.
 * @param cosBeta The cosine of the beta angle.
 * @param sinBeta The sine of the beta angle.
 * @param cosGamma The cosine of the gamma angle.
 * @param sinGamma The sine of the gamma angle.
 * @param m the 3x3 rotation matrix result.
 *
 */
void blasBuildRotMatAngles(
    const float cosAlpha, const float sinAlpha,
    const float cosBeta, const float sinBeta,
    const float cosGamma, const float sinGamma,
    float *m);

/**
 * This function is an interface for the blasBuildRotMatDir(), that creates
 * the rotation matrix needed to transform the FROM vector into the TO vector.
 *
 * @param from The vector we want to transform.
 * @param to The transformed vector.
 * @param mat The resulting transformation matrix for transforming #from into
 *      #to.
 */
void blasBuildRotMatFromTo(float *from, float *to, float *mat);

/**
 * Function that multiplies two 3x3 matrixes, AxB=AB.
 *
 * @param a First matrix.
 * @param b Second matrix.
 * @param ab The product of the a and b matrix.
 */
void blasMatMatProd(const float *a, const float *b, float *ab);

/**
 * Function that transforms a vector using matrix vector/matrix product.
 * The function multiplies a 3D vector with a 3x3 matrix.
 *
 * @param x Vector to be multiplied.
 * @param m Transformation matrix.
 * @param mx Transformed vector.
 */
void blasVecMatrixProd(const float *x, const float *m, float *mx);

/**
 * 3D Vector addition.
 *
 * @param x First operand.
 * @param y Second operand.
 * @param z Result vector of x + y.
 */
void blasAdd(const float *x, const float *y, float *z);

/**
 * 3D Vector substraction.
 *
 * @param x First operand.
 * @param y Second operand.
 * @param z Result vector of x - y.
 */
void blasSubstract(const float *x, const float *y, float *z);

/**
 * 3D vector dot product.
 *
 * @param x First operand.
 * @param y Second operand.
 * @param z Result vector of x dot y.
 */
float blasDot(const float *x, const float *y);

/**
 * 3D vector cross product.
 *
 * @param x First operand.
 * @param y Second operand.
 * @param z Result vector of x cross y.
 */
void blasCross(const float *x, const float *y, float *z);

/**
 * 3D vector copy.
 *
 * @param x Source vector.
 * @param y Destination vector.
 */
void blasCopy(const float *x, float *y);

/**
 * 3D vector scaling
 *
 * @param x Source operand.
 * @param k Scale factor.
 * @param kx Result vector of kX.
 */
void blasScale(const float *x, register const float k, float *kx);

/**
 * 3D vector inversion.
 *
 * @param x Source vector.
 * @param invx Inverted source vector.
 */
void blasInvert(const float *x, float *invx);

/**
 * Square root approximation using the babilonean square root method.
 *
 * @param x The number from which we want to retrieve the square root.
 * @result The square root approximation
 */
float blasFastBabSqrt(const float x);

/**
 * Inverse Square root approximation using the Quake 3 method.
 *
 * @param x The number from which we want to retrieve the inverse square root.
 * @result The inverse square root approximation
 */
float blasFastInvSqrt(const float x);

/**
 * 3D Vector normalization.
 *
 * @param x Vector to be normalized.
 */
void blasNormalize(float *x);

/**
 * 3D Vector fast normalization, using the babilonean square root approximation.
 *
 * @param x Vector to be normalized.
 */
void blasFastNormalize(float *x);

/**
 * 3D Vector very fast normalization, using the Quake 3 inverse square
 * approximation.
 *
 * @param x Vector to be normalized.
 */
void blasVeryFastNormalize(float *x);

/**
 * L2 Norm of a 3D vector
 * @param x Vector from which we want the norm.
 */
float blasNrm2(const float *x);

#endif /* BLAS_H_ */
