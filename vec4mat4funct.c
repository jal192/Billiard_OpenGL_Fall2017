/*
	vec4mat4funct.c

	Created on: Sept 17, 2017
	Author: Jason Ly
*/

#include "vec4mat4headers.h"
#include <stdio.h>
#include <math.h>

// NOTE: All vectors are being treated as column vectors and
//		 all matrices are treated as being column major
// Vector multiplication was not coded because it's undefined

// Vectors of Size 4
// Vector Addition
vec4 v4_add(vec4 v1, vec4 v2) {
	vec4 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	result.w = v1.w + v2.w;
	return result;
}

// Vector Subtraction
vec4 v4_sub(vec4 v1, vec4 v2) {
	vec4 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	result.w = v1.w - v2.w;
	return result;
}

// Vector Scalar Multiplication
vec4 v4_scalar_mult(vec4 v, GLfloat s) {
	vec4 result;
	result.x = s * v.x;
	result.y = s * v.y;
	result.z = s * v.z;
	result.w = s * v.w;
	return result;
}

// Dot Product
GLfloat v4_dot_prod(vec4 v1, vec4 v2) {
	GLfloat result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
	return result;
}

// Cross Product
// Since cross product of vec4 is no possible, do cross product using
// x,y,z and set w to 0
vec4 v4_cross_prod(vec4 v1, vec4 v2) {
	vec4 result;
	result.x = (v1.y*v2.z) - (v1.z*v2.y);
	result.y = -((v1.x*v2.z) - (v1.z*v2.x));
	result.z = (v1.x*v2.y) - (v1.y*v2.x);
	result.w = 0;
	return result;
}

// Calculate the Unit Vector
vec4 v4_unit_vec(vec4 v1) {
	GLfloat magnitude = (GLfloat)sqrt(pow(v1.x, 2) + pow(v1.y, 2) + pow(v1.z, 2));
	vec4 result = v4_scalar_mult(v1, 1 / magnitude);
	return result;
}

GLfloat v4_length(vec4 v1) {
	GLfloat magnitude = sqrt(pow(v1.x, 2) + pow(v1.y, 2) + pow(v1.z, 2));
	return magnitude;
}

// Print Vector, printed as column vector
void print_vec4(vec4 v) {
	printf("%f\n%f\n%f\n%f\n", v.x, v.y, v.z, v.w);
}


// Matrix Functions For 4x4
// Matrix Addition
mat4 m4_add(mat4 m1, mat4 m2) {
	mat4 result;
	result.x = v4_add(m1.x, m2.x);
	result.y = v4_add(m1.y, m2.y);
	result.z = v4_add(m1.z, m2.z);
	result.w = v4_add(m1.w, m2.w);
	return result;
}

// Matrix Subtraction
mat4 m4_sub(mat4 m1, mat4 m2) {
	mat4 result;
	result.x = v4_sub(m1.x, m2.x);
	result.y = v4_sub(m1.y, m2.y);
	result.z = v4_sub(m1.z, m2.z);
	result.w = v4_sub(m1.w, m2.w);
	return result;
}

// Matrix Multiplication
mat4 m4_mult(mat4 m1, mat4 m2) {
	mat4 result;
	// Transpose the first matrix because matrix multiplication involves
	// performing dot product on the rows of the left matrix by the columns 
	// of the right matrix. Transposing sets up the matrices in this way.
	// Makes coding the calculation a lot simpler
	m1 = m4_transpose(m1);

	result.x.x = v4_dot_prod(m1.x, m2.x);
	result.x.y = v4_dot_prod(m1.y, m2.x);
	result.x.z = v4_dot_prod(m1.z, m2.x);
	result.x.w = v4_dot_prod(m1.w, m2.x);

	result.y.x = v4_dot_prod(m1.x, m2.y);
	result.y.y = v4_dot_prod(m1.y, m2.y);
	result.y.z = v4_dot_prod(m1.z, m2.y);
	result.y.w = v4_dot_prod(m1.w, m2.y);

	result.z.x = v4_dot_prod(m1.x, m2.z);
	result.z.y = v4_dot_prod(m1.y, m2.z);
	result.z.z = v4_dot_prod(m1.z, m2.z);
	result.z.w = v4_dot_prod(m1.w, m2.z);

	result.w.x = v4_dot_prod(m1.x, m2.w);
	result.w.y = v4_dot_prod(m1.y, m2.w);
	result.w.z = v4_dot_prod(m1.z, m2.w);
	result.w.w = v4_dot_prod(m1.w, m2.w);

	return result;
}

// Matrix Multiplied by Vector
vec4 m4_mult_vec(mat4 m, vec4 v) {
	vec4 result;
	// Transpose the first matrix because matrix multiplication involves
	// performing dot product on the rows of the left matrix by the column 
	// of the right vector. Transposing sets up the matrices in this way.
	// Makes coding the calculation a lot simpler
	m = m4_transpose(m);

	result.x = v4_dot_prod(m.x, v);
	result.y = v4_dot_prod(m.y, v);
	result.z = v4_dot_prod(m.z, v);
	result.w = v4_dot_prod(m.w, v);

	return result;
}

// Matrix Scalar Multiplication
mat4 m4_scalar_mult(mat4 m, GLfloat s) {
	mat4 result;
	result.x = v4_scalar_mult(m.x, s);
	result.y = v4_scalar_mult(m.y, s);
	result.z = v4_scalar_mult(m.z, s);
	result.w = v4_scalar_mult(m.w, s);
	return result;
}

// Matrix Inverse
mat4 m4_inverse(mat4 m) {
	mat4 minor = m4_minor(m);
	mat4 cofactor = m4_cofactor(minor);
	mat4 transpose = m4_transpose(cofactor);
	GLfloat determinant = m4_determinant(m);
	mat4 inverse = m4_scalar_mult(transpose, 1 / determinant);
	return inverse;
}

// Matrix Transpose
mat4 m4_transpose(mat4 m) {
	vec4 v1 = { m.x.x, m.y.x, m.z.x, m.w.x };
	vec4 v2 = { m.x.y, m.y.y, m.z.y, m.w.y };
	vec4 v3 = { m.x.z, m.y.z, m.z.z, m.w.z };
	vec4 v4 = { m.x.w, m.y.w, m.z.w, m.w.w };
	mat4 result = { v1, v2, v3, v4 };
	return result;
}

// Print Matrix, printed as column major
void print_mat4(mat4 m) {
	printf("%f, %f, %f, %f \n", m.x.x, m.y.x, m.z.x, m.w.x);
	printf("%f, %f, %f, %f \n", m.x.y, m.y.y, m.z.y, m.w.y);
	printf("%f, %f, %f, %f \n", m.x.z, m.y.z, m.z.z, m.w.z);
	printf("%f, %f, %f, %f \n", m.x.w, m.y.w, m.z.w, m.w.w);
}

// Find the minor of a matrix
mat4 m4_minor(mat4 m) {
	GLfloat m11 = (m.y.y)*(m.z.z)*(m.w.w) + (m.z.y)*(m.w.z)*(m.y.w) + (m.w.y)*(m.y.z)*(m.z.w) - (m.y.w)*(m.z.z)*(m.w.y) - (m.z.w)*(m.w.z)*(m.y.y) - (m.w.w)*(m.y.z)*(m.z.y);
	GLfloat m12 = (m.y.x)*(m.z.z)*(m.w.w) + (m.z.x)*(m.w.z)*(m.y.w) + (m.w.x)*(m.y.z)*(m.z.w) - (m.y.w)*(m.z.z)*(m.w.x) - (m.z.w)*(m.w.z)*(m.y.x) - (m.w.w)*(m.y.z)*(m.z.x);
	GLfloat m13 = (m.y.x)*(m.z.y)*(m.w.w) + (m.z.x)*(m.w.y)*(m.y.w) + (m.w.x)*(m.y.y)*(m.z.w) - (m.y.w)*(m.z.y)*(m.w.x) - (m.z.w)*(m.w.y)*(m.y.x) - (m.w.w)*(m.y.y)*(m.z.x);
	GLfloat m14 = (m.y.x)*(m.z.y)*(m.w.z) + (m.z.x)*(m.w.y)*(m.y.z) + (m.w.x)*(m.y.y)*(m.z.z) - (m.y.z)*(m.z.y)*(m.w.x) - (m.z.z)*(m.w.y)*(m.y.x) - (m.w.z)*(m.y.y)*(m.z.x);
	vec4 v1 = { m11, m12, m13, m14 };

	GLfloat m21 = (m.x.y)*(m.z.z)*(m.w.w) + (m.z.y)*(m.w.z)*(m.x.w) + (m.w.y)*(m.x.z)*(m.z.w) - (m.x.w)*(m.z.z)*(m.w.y) - (m.z.w)*(m.w.z)*(m.x.y) - (m.w.w)*(m.x.z)*(m.z.y);
	GLfloat m22 = (m.x.x)*(m.z.z)*(m.w.w) + (m.z.x)*(m.w.z)*(m.x.w) + (m.w.x)*(m.x.z)*(m.z.w) - (m.x.w)*(m.z.z)*(m.w.x) - (m.z.w)*(m.w.z)*(m.x.x) - (m.w.w)*(m.x.z)*(m.z.x);
	GLfloat m23 = (m.x.x)*(m.z.y)*(m.w.w) + (m.z.x)*(m.w.y)*(m.x.w) + (m.w.x)*(m.x.y)*(m.z.w) - (m.x.w)*(m.z.y)*(m.w.x) - (m.z.w)*(m.w.y)*(m.x.x) - (m.w.w)*(m.x.y)*(m.z.x);
	GLfloat m24 = (m.x.x)*(m.z.y)*(m.w.z) + (m.z.x)*(m.w.y)*(m.x.z) + (m.w.x)*(m.x.y)*(m.z.z) - (m.x.z)*(m.z.y)*(m.w.x) - (m.z.z)*(m.w.y)*(m.x.x) - (m.w.z)*(m.x.y)*(m.z.x);
	vec4 v2 = { m21, m22, m23, m24 };

	GLfloat m31 = (m.x.y)*(m.y.z)*(m.w.w) + (m.y.y)*(m.w.z)*(m.x.w) + (m.w.y)*(m.x.z)*(m.y.w) - (m.x.w)*(m.y.z)*(m.w.y) - (m.y.w)*(m.w.z)*(m.x.y) - (m.w.w)*(m.x.z)*(m.y.y);
	GLfloat m32 = (m.x.x)*(m.y.z)*(m.w.w) + (m.y.x)*(m.w.z)*(m.x.w) + (m.w.x)*(m.x.z)*(m.y.w) - (m.x.w)*(m.y.z)*(m.w.x) - (m.y.w)*(m.w.z)*(m.x.x) - (m.w.w)*(m.x.z)*(m.y.x);
	GLfloat m33 = (m.x.x)*(m.y.y)*(m.w.w) + (m.y.x)*(m.w.y)*(m.x.w) + (m.w.x)*(m.x.y)*(m.y.w) - (m.x.w)*(m.y.y)*(m.w.x) - (m.y.w)*(m.w.y)*(m.x.x) - (m.w.w)*(m.x.y)*(m.y.x);
	GLfloat m34 = (m.x.x)*(m.y.y)*(m.w.z) + (m.y.x)*(m.w.y)*(m.x.z) + (m.w.x)*(m.x.y)*(m.y.z) - (m.x.z)*(m.y.y)*(m.w.x) - (m.y.z)*(m.w.y)*(m.x.x) - (m.w.z)*(m.x.y)*(m.y.x);
	vec4 v3 = { m31, m32, m33, m34 };

	GLfloat m41 = (m.x.y)*(m.y.z)*(m.z.w) + (m.y.y)*(m.z.z)*(m.x.w) + (m.z.y)*(m.x.z)*(m.y.w) - (m.x.w)*(m.y.z)*(m.z.y) - (m.y.w)*(m.z.z)*(m.x.y) - (m.z.w)*(m.x.z)*(m.y.y);
	GLfloat m42 = (m.x.x)*(m.y.z)*(m.z.w) + (m.y.x)*(m.z.z)*(m.x.w) + (m.z.x)*(m.x.z)*(m.y.w) - (m.x.w)*(m.y.z)*(m.z.x) - (m.y.w)*(m.z.z)*(m.x.x) - (m.z.w)*(m.x.z)*(m.y.x);
	GLfloat m43 = (m.x.x)*(m.y.y)*(m.z.w) + (m.y.x)*(m.z.y)*(m.x.w) + (m.z.x)*(m.x.y)*(m.y.w) - (m.x.w)*(m.y.y)*(m.z.x) - (m.y.w)*(m.z.y)*(m.x.x) - (m.z.w)*(m.x.y)*(m.y.x);
	GLfloat m44 = (m.x.x)*(m.y.y)*(m.z.z) + (m.y.x)*(m.z.y)*(m.x.z) + (m.z.x)*(m.x.y)*(m.y.z) - (m.x.z)*(m.y.y)*(m.z.x) - (m.y.z)*(m.z.y)*(m.x.x) - (m.z.z)*(m.x.y)*(m.y.x);
	vec4 v4 = { m41, m42, m43, m44 };

	mat4 minor = { v1, v2, v3, v4 };
	return minor;
}

// Find the cofactor of a matrix
mat4 m4_cofactor(mat4 m) {
	vec4 v1 = { m.x.x, -m.x.y, m.x.z, -m.x.w };
	vec4 v2 = { -m.y.x, m.y.y, -m.y.z, m.y.w };
	vec4 v3 = { m.z.x, -m.z.y, m.z.z, -m.z.w };
	vec4 v4 = { -m.w.x, m.w.y, -m.w.z, m.w.w };
	mat4 cofactor = { v1, v2, v3, v4 };

	return cofactor;
}

// Calculate determinant of matrix
// If determinant returns 0 then an inverse doesn't exist
GLfloat m4_determinant(mat4 m) {
	GLfloat m11 = (m.y.y)*(m.z.z)*(m.w.w) + (m.z.y)*(m.w.z)*(m.y.w) + (m.w.y)*(m.y.z)*(m.z.w) - (m.y.w)*(m.z.z)*(m.w.y) - (m.z.w)*(m.w.z)*(m.y.y) - (m.w.w)*(m.y.z)*(m.z.y);
	GLfloat m21 = (m.x.y)*(m.z.z)*(m.w.w) + (m.z.y)*(m.w.z)*(m.x.w) + (m.w.y)*(m.x.z)*(m.z.w) - (m.x.w)*(m.z.z)*(m.w.y) - (m.z.w)*(m.w.z)*(m.x.y) - (m.w.w)*(m.x.z)*(m.z.y);
	GLfloat m31 = (m.x.y)*(m.y.z)*(m.w.w) + (m.y.y)*(m.w.z)*(m.x.w) + (m.w.y)*(m.x.z)*(m.y.w) - (m.x.w)*(m.y.z)*(m.w.y) - (m.y.w)*(m.w.z)*(m.x.y) - (m.w.w)*(m.x.z)*(m.y.y);
	GLfloat m41 = (m.x.y)*(m.y.z)*(m.z.w) + (m.y.y)*(m.z.z)*(m.x.w) + (m.z.y)*(m.x.z)*(m.y.w) - (m.x.w)*(m.y.z)*(m.z.y) - (m.y.w)*(m.z.z)*(m.x.y) - (m.z.w)*(m.x.z)*(m.y.y);

	GLfloat determinant = m.x.x*m11 - m.y.x*m21 + m.z.x*m31 - m.w.x*m41;

	return determinant;
}

mat4 m4_identity() {
	mat4 result = { 
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 } };

	return result;
}
