/*
	vec4mat4headers.h

	Created on: Sept 17, 2017
	Author: Jason Ly
*/

#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include <math.h>

// NOTE: All vectors are being treated as column vectors and
//		 all matrices are treated as being column major
// Vector multiplication was not coded because it's undefined

// Define Vector
typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat w;
} vec4;

// Define Matrix
typedef struct {
	vec4 x;
	vec4 y;
	vec4 z;
	vec4 w;
} mat4;

vec4 v4_add(vec4 v1, vec4 v2);
vec4 v4_sub(vec4 v1, vec4 v2);
vec4 v4_scalar_mult(vec4 v, GLfloat s);
GLfloat v4_dot_prod(vec4 v1, vec4 v2);
vec4 v4_cross_prod(vec4 v1, vec4 v2);
vec4 v4_unit_vec(vec4 v1);
GLfloat v4_length(vec4 v1);
void print_vec4(vec4 v);

mat4 m4_add(mat4 m1, mat4 m2);
mat4 m4_sub(mat4 m1, mat4 m2);
mat4 m4_mult(mat4 m1, mat4 m2);
vec4 m4_mult_vec(mat4 m, vec4 v);
mat4 m4_scalar_mult(mat4 m, GLfloat s);
mat4 m4_inverse(mat4 m);
mat4 m4_transpose(mat4 m);
void print_mat4(mat4 m);
mat4 m4_minor(mat4 m);
mat4 m4_cofactor(mat4 m);
GLfloat m4_determinant(mat4 m);
mat4 m4_identity();
