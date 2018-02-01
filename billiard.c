/*
	billiard.c

	Created on: Nov 9, 2017
	Author: Jason Ly
*/

#ifdef __APPLE__  // include Mac OS X versions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include "vec4mat4headers.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#include "initShader.h"
#include <string.h>
#include <time.h>
#include <windows.h>

// Function signatures
mat4 rotate_about_y(float theta);
mat4 scale(float sx, float sy, float sz);
mat4 translate(float dx, float dy, float dz);
mat4 look_at(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat atx, GLfloat aty, GLfloat atz, GLfloat upx, GLfloat upy, GLfloat upz);
mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val);
vec4 calculateEyePoint(GLfloat theta, GLfloat phi, GLfloat radius);
vec4 product(vec4 u, vec4 v);

typedef struct
{
	vec4 reflect_ambient;
	vec4 reflect_diffuse;
	vec4 reflect_specular;
	GLfloat shininess;
} material;

// Number of vertices in the entire world
int num_vertices = (6 * 14694) + 36;

vec4 vertices[88200];
vec4 normal_vec[88200];

int num_sphere_vertices = 14694;
vec4 sphere_vertices[14694];

float radian_to_degree = 180.0 / M_PI;
float DegreesToRadians = M_PI / 180.0;

// Set the screen width and height
int screen_width = 512;
int screen_height = 512;

// Moves the window to the right side of the screen
int window_position_width = 900;
int window_position_height = 100;

// Stores the radius or what half the sides of the screen is
int mid_width_x;
int mid_height_y;

GLuint ctm_location;
GLuint model_view_location;
GLuint projection_location;
GLuint isShadow_location;
GLuint ap_location;
GLuint dp_location;
GLuint sp_location;
GLuint light_position_location;
GLuint ac_location;
GLuint al_location;
GLuint aq_location;
GLuint shininess_location;

vec4 AmbientProduct;
vec4 DiffuseProduct;
vec4 SpecularProduct;

float attenuation_constant = 0.0;
float attenuation_linear = 0.0;
float attenuation_quadratic = 0.1;

// Translation matrix
mat4 mv_matrix =
{ { 1.0, 0.0, 0.0, 0.0 },
{ 0.0, 1.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0, 0.0 },
{ 0.0, 0.0, 0.0, 1.0 } };

// Projection matrix
mat4 projection =
{ { 1.0, 0.0, 0.0, 0.0 },
{ 0.0, 1.0, 0.0, 0.0 },
{ 0.0, 0.0, 1.0, 0.0 },
{ 0.0, 0.0, 0.0, 1.0 } };

// Increment amounts when making the sphere, smaller the number, smoother the sphere
float theta_increment = 5.0;
float phi_increment = 5.0;

float eye_x, eye_y, eye_z;
float at_x, at_y, at_z;

// Starting location of the light
float light_x = 0.0f;
float light_y = 3.0f;
float light_z = 0.0f;

vec4 light_position;

// Scaling factor to change size of objects
GLfloat sphere_scaling_factor = 0.5;
GLfloat light_scaling_factor = 0.1;

mat4 table_ctm;
mat4 red_sphere_ctm;
mat4 green_sphere_ctm;
mat4 blue_sphere_ctm;
mat4 yellow_sphere_ctm;
mat4 orange_sphere_ctm;
mat4 white_sphere_ctm;

// Translation matrix that moves the spheres to the correct position
mat4 red_sphere_translation;
mat4 green_sphere_translation;
mat4 blue_sphere_translation;
mat4 yellow_sphere_translation;
mat4 orange_sphere_translation;
mat4 light_sphere_translation;

// Keep track of the angle the spheres are located at
GLfloat green_angle = 90.0f;
GLfloat blue_angle = 90.0f;
GLfloat yellow_angle = 90.0f;
GLfloat orange_angle = 90.0f;

// Set angle amount that the each sphere rotates by
GLfloat green_rotation_speed = 0.02f;
GLfloat blue_rotation_speed = 0.03f;
GLfloat yellow_rotation_speed = 0.04f;
GLfloat orange_rotation_speed = 0.05f;

// Store the center point of each object, used to calculate the vNormals
vec4 object_centers[7] = {
	{ 0.0, -11.0, 0.0, 1.0 },	// Table center 
	{ 0.0, 0.5, 0.0, 1.0 },		// Red sphere center
	{ 1.0, 0.5, 0.0, 1.0 },		// Green sphere center
	{ 2.0, 0.5, 0.0, 1.0 },		// Blue sphere center
	{ 3.0, 0.5, 0.0, 1.0 },		// Yellow sphere center
	{ 4.0, 0.5, 0.0, 1.0 },		// Orange sphere center
	{ 0.0, 3.0, 0.0, 1.0 },		// Light sphere center
};

// Index that keeps track of which center point being used
int object_index = 0;

// Values for frustum
GLfloat fx_neg = -1.0f;
GLfloat fx_pos = 0.5f;
GLfloat fy_neg = 0.0f;
GLfloat fy_pos = 0.5f;
GLfloat fz_pos = -0.7f;
GLfloat fz_neg = -100.0f;

// Settings for light
vec4 light_diffuse = { 1.0, 1.0, 1.0, 1.0 };
vec4 light_specular = { 1.0, 1.0, 1.0, 1.0 };
vec4 light_ambient = { 0.2, 0.2, 0.2, 1.0 };

// Materials settings for each sphere
material ball_materials[6] = {
	{ { 1.0, 0.0, 0.0, 1.0 },{ 1.0, 0.0, 0.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 }, 10 },
	{ { 0.0, 1.0, 0.0, 1.0 },{ 0.0, 1.0, 0.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 }, 10 },
	{ { 0.0, 0.0, 1.0, 1.0 },{ 0.0, 0.0, 1.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 }, 10 },
	{ { 1.0, 1.0, 0.0, 1.0 },{ 1.0, 1.0, 0.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 }, 10 },
	{ { 1.0, 0.5, 0.0, 1.0 },{ 1.0, 0.5, 0.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 }, 10 },
	{ { 1.0, 1.0, 1.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 },{ 1.0, 1.0, 1.0, 1.0 }, 10 }
};

// Material setting for table
material table_material = {
	{ 0.0f, 0.686f, 0.0f, 1.0f },{ 0.0f, 0.686f, 0.0f, 1.0f },{ 1.0, 1.0, 1.0, 1.0 }, 10
};

// Vertices for ground/floor cube
vec4 ground_vertices[36] = {
	{ -20.0f, 0.0f, 20.0f, 1.0f }, { -20.0f, -2.0f, 20.0f, 1.0f }, { 20.0f, -2.0f, 20.0f, 1.0f },	// Front Side of Cube
	{ 20.0f, 0.0f, 20.0f, 1.0f }, { -20.0f, 0.0f, 20.0f, 1.0f }, { 20.0f, -2.0f, 20.0f, 1.0f }, 	// Front Side of Cube
	{ -20.0f, -2.0f, -20.0f, 1.0f }, { -20.0f, -2.0f, 20.0f, 1.0f }, { -20.0f, 0.0f, 20.0f, 1.0f },	// Left Side of Cube
	{ -20.0f, -2.0f, -20.0f, 1.0f }, { -20.0f, 0.0f, 20.0f, 1.0f }, { -20.0f, 0.0f, -20.0f, 1.0f },	// Left Side of Cube
	{ 20.0f, 0.0f, -20.0f, 1.0f }, { -20.0f, -2.0f,-20.0f, 1.0f }, { -20.0f, 0.0f, -20.0f, 1.0f },	// Back Sidde of Cube
	{ 20.0f, 0.0f, -20.0f, 1.0f }, { 20.0f, -2.0f, -20.0f, 1.0f }, { -20.0f, -2.0f, -20.0f, 1.0f },	// Back Side of Cube
	{ 20.0f, -2.0f, 20.0f, 1.0f }, { -20.0f, -2.0f, -20.0f, 1.0f }, { 20.0f, -2.0f, -20.0f, 1.0f },	// Bottom Side of Cube
	{ 20.0f, -2.0f, 20.0f, 1.0f }, { -20.0f, -2.0f, 20.0f, 1.0f }, { -20.0f, -2.0f, -20.0f, 1.0f },	// Bottom Side of Cube
	{ 20.0f, 0.0f, 20.0f, 1.0f }, { 20.0f, -2.0f,-20.0f, 1.0f }, { 20.0f, 0.0f, -20.0f, 1.0f },		// Right side of Cube
	{ 20.0f, -2.0f, -20.0f, 1.0f }, { 20.0f, 0.0f, 20.0f, 1.0f }, { 20.0f, -2.0f, 20.0f, 1.0f },	// Right Side of Cube
	{ 20.0f, 0.0f, 20.0f, 1.0f }, { -20.0f, 0.0f, -20.0f, 1.0f }, { -20.0f, 0.0f, 20.0f, 1.0f },	// Top Side of Cube
	{ 20.0f, 0.0f, 20.0f, 1.0f }, { 20.0f, 0.0f, -20.0f, 1.0f }, { -20.0f, 0.0f, -20.0f, 1.0f } 	// Top Side of Cube
};

// Starting values for theta, phi, and r for eye point
GLfloat theta = 90.0f;
GLfloat phi = 70.0f;
GLfloat radius = 10.0f;

int enableIdle = 0;

void init(void) {
	// Print out controls to the user
	printf(
		"Below is a list of the controls:\n"
		"---------------------------------------------------------------------\n"
		"Note: lowercase decreases the value and uppercase increases the value\n"
		"---------------------------------------------------------------------\n"
		"Moving the eye: Adjust Theta: 't/T', Phi: 'p/P', Radius: 'r/R'\n"
		"Moving the light: Adjust X: 'x/X', Y: 'y/Y', Z: 'z/Z'\n"
		"Start animation: spacebar\n"
		"Quit program: 'q'\n\n"
	);

	// Initialize all ctm matrices to identity matrix
	table_ctm = m4_identity();
	red_sphere_ctm = m4_identity();
	green_sphere_ctm = m4_identity();
	blue_sphere_ctm = m4_identity();
	yellow_sphere_ctm = m4_identity();
	orange_sphere_ctm = m4_identity();
	white_sphere_ctm = m4_identity();

	// Generate Initial Sphere Vertices

	// This code below is from the book and example code provided by Tan
	// The code has been modified to use triangles instead of quads
	// Get all of the sphere except the poles
	float phir;
	float phir20;
	float thetar;
	int k = 0;

	// Generate half of the triangles for the quad
	for (float phi = -80.0; phi <= 80.0; phi += phi_increment) {
		phir = phi*DegreesToRadians;
		phir20 = (float)(phi + phi_increment)*DegreesToRadians;
		for (float theta = -180.0; theta < 180.0; theta += theta_increment) {
			thetar = theta*DegreesToRadians;
			sphere_vertices[k].x = sin(thetar)*cos(phir);
			sphere_vertices[k].y = cos(thetar)*cos(phir);
			sphere_vertices[k].z = sin(phir);
			sphere_vertices[k].w = 1.0;
			k++;
			sphere_vertices[k].x = sin(thetar)*cos(phir20);
			sphere_vertices[k].y = cos(thetar)*cos(phir20);
			sphere_vertices[k].z = sin(phir20);
			sphere_vertices[k].w = 1.0;
			k++;
			thetar = (theta + theta_increment)*DegreesToRadians;
			sphere_vertices[k].x = sin(thetar)*cos(phir);
			sphere_vertices[k].y = cos(thetar)*cos(phir);
			sphere_vertices[k].z = sin(phir);
			sphere_vertices[k].w = 1.0;
			k++;
		}
	}

	// Generate the other half of the triangles for the quads
	for (float phi = -80.0; phi <= 80.0; phi += phi_increment) {
		phir = phi*DegreesToRadians;
		phir20 = (float)(phi + phi_increment)*DegreesToRadians;
		for (float theta = -180.0; theta < 180.0; theta += theta_increment) {
			thetar = theta*DegreesToRadians;
			sphere_vertices[k].x = sin(thetar)*cos(phir20);
			sphere_vertices[k].y = cos(thetar)*cos(phir20);
			sphere_vertices[k].z = sin(phir20);
			sphere_vertices[k].w = 1.0;
			k++;
			thetar = (theta + theta_increment)*DegreesToRadians;
			sphere_vertices[k].x = sin(thetar)*cos(phir20);
			sphere_vertices[k].y = cos(thetar)*cos(phir20);
			sphere_vertices[k].z = sin(phir20);
			sphere_vertices[k].w = 1.0;
			k++;
			sphere_vertices[k].x = sin(thetar)*cos(phir);
			sphere_vertices[k].y = cos(thetar)*cos(phir);
			sphere_vertices[k].z = sin(phir);
			sphere_vertices[k].w = 1.0;
			k++;
		}
	}

	// Get the top and bottom
	float sin80 = sin(80.0*DegreesToRadians);
	float cos80 = cos(80.0*DegreesToRadians);

	for (float theta = -180.0; theta <= 180.0; theta += theta_increment)
	{
		// Draw the triangle, taking the first point on the outside
		// of the circle, then the center then the next point
		float thetar = theta*DegreesToRadians;
		sphere_vertices[k].x = sin(thetar)*cos80;
		sphere_vertices[k].y = cos(thetar)*cos80;
		sphere_vertices[k].z = sin80;
		sphere_vertices[k].w = 1.0;
		k++;
		sphere_vertices[k].x = 0;
		sphere_vertices[k].y = 0;
		sphere_vertices[k].z = 1;
		sphere_vertices[k].w = 1.0;
		k++;
		float thetatemp = (theta + theta_increment)*DegreesToRadians;
		sphere_vertices[k].x = sin(thetatemp)*cos80;
		sphere_vertices[k].y = cos(thetatemp)*cos80;
		sphere_vertices[k].z = sin80;
		sphere_vertices[k].w = 1.0;
		k++;
	}

	for (float theta = -180.0; theta <= 180.0; theta += theta_increment)
	{
		// Draw the triangle, taking the first point on the outside
		// of the circle, then the center then the next point
		float temp = theta;

		float thetatemp = (temp + theta_increment)*DegreesToRadians;
		sphere_vertices[k].x = sin(thetatemp)*cos80;
		sphere_vertices[k].y = cos(thetatemp)*cos80;
		sphere_vertices[k].z = -sin80;
		sphere_vertices[k].w = 1.0;
		k++;

		sphere_vertices[k].x = 0;
		sphere_vertices[k].y = 0;
		sphere_vertices[k].z = -1;
		sphere_vertices[k].w = 1.0;
		k++;

		float thetar = temp*DegreesToRadians;
		sphere_vertices[k].x = sin(thetar)*cos80;
		sphere_vertices[k].y = cos(thetar)*cos80;
		sphere_vertices[k].z = -sin80;
		sphere_vertices[k].w = 1.0;
		k++;
	}

	printf("Number of sphere_vertices: %d\n", k);

	int vIndex = 0;

	// Copy the ground cube into the world vertices
	for (int i = 0; i < 36; i++) {
		vertices[vIndex] = ground_vertices[i];
		normal_vec[vIndex] = v4_unit_vec(v4_sub(ground_vertices[i], object_centers[object_index]));
		vIndex++;
	}
	object_index++;

	// Array that will store the translation matrices to move the sphere to the desired location
	// Putting them in an array makes it easier to iterate through all the different spheres that are needed
	mat4 mat_array[5];

	// Move spheres next to each other in a line
	red_sphere_translation = translate(0.0f, 0.5f, 0.0f);
	green_sphere_translation = translate(1.0f, 0.5f, 0.0f);
	blue_sphere_translation = translate(2.0f, 0.5f, 0.0f);
	yellow_sphere_translation = translate(3.0f, 0.5f, 0.0f);
	orange_sphere_translation = translate(4.0f, 0.5f, 0.0f);

	// Put the translation matrices into an array so they can be easily referenced in for loop
	mat_array[0] = red_sphere_translation;
	mat_array[1] = green_sphere_translation;
	mat_array[2] = blue_sphere_translation;
	mat_array[3] = yellow_sphere_translation;
	mat_array[4] = orange_sphere_translation;

	// Create scaling matrix
	mat4 sphere_scaling_matrix = scale(sphere_scaling_factor, sphere_scaling_factor, sphere_scaling_factor);
	mat4 temp_tr_matrix;

	// Make 4 copies of the cube and put it in different corners of the screen
	for (int i = 0; i < 5; i++) {
		temp_tr_matrix = m4_identity();

		// Get finalized translation matrix that will move and scale the sphere
		temp_tr_matrix = m4_mult(sphere_scaling_matrix, temp_tr_matrix);
		temp_tr_matrix = m4_mult(mat_array[i], temp_tr_matrix);

		// For each vertex of the sphere apply the finalized translation matrix
		// Then use result to calculate the vNormal based on the center of the sphere
		for (int j = 0; j < num_sphere_vertices; j++) {
			vertices[vIndex] = m4_mult_vec(temp_tr_matrix, sphere_vertices[j]);
			normal_vec[vIndex] = v4_unit_vec(v4_sub(vertices[vIndex], object_centers[object_index]));
			vIndex++;
		}

		// Increment index to use the next object center
		object_index++;
	}

	// Generate sphere that represents the light
	mat4 light_scaling_matrix = scale(light_scaling_factor, light_scaling_factor, light_scaling_factor);
	light_sphere_translation = translate(light_x, light_y, light_z);
	temp_tr_matrix = m4_identity();

	// Get finalized translation matrix that will move and scale the sphere
	temp_tr_matrix = m4_mult(light_scaling_matrix, temp_tr_matrix);
	temp_tr_matrix = m4_mult(light_sphere_translation, temp_tr_matrix);

	// For each vertex of the sphere apply the finalized translation matrix
	for (int j = 0; j < num_sphere_vertices; j++) {
		vertices[vIndex] = m4_mult_vec(temp_tr_matrix, sphere_vertices[j]);
		normal_vec[vIndex] = v4_unit_vec(v4_sub(object_centers[object_index], vertices[vIndex]));
		vIndex++;
	}

	printf("vIndex: %d\n", vIndex);

	// Calculate the eye point based on the theta, phi and radius
	vec4 eye = calculateEyePoint(theta, phi, radius);
	eye_x = eye.x;
	eye_y = eye.y;
	eye_z = eye.z;
	at_x = 0.0;
	at_y = 0.0;
	at_z = 0.0;

	// Update model view with user input
	mv_matrix = look_at(eye_x, eye_y, eye_z, at_x, at_y, at_z, 0.0, 1.0, 0.0);

	// Set frustum
	mat4 frustum_matrix = frustum(fx_neg, fx_pos, fy_neg, fy_pos, fz_pos, fz_neg);
	projection = frustum_matrix;

	// Set initial light position
	light_position.x = light_x;
	light_position.y = light_y;
	light_position.z = light_z;
	light_position.w = 1.0;

	// Calculate the center of the screen
	mid_width_x = floor(screen_width / 2.0);
	mid_height_y = floor(screen_height / 2.0);

	GLuint program = initShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normal_vec), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normal_vec), normal_vec);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(normal_vec));

	ctm_location = glGetUniformLocation(program, "ctm");
	model_view_location = glGetUniformLocation(program, "model_view_matrix");
	projection_location = glGetUniformLocation(program, "projection_matrix");
	isShadow_location = glGetUniformLocation(program, "isShadow");
	
	ap_location = glGetUniformLocation(program, "AmbientProduct");
	dp_location = glGetUniformLocation(program, "DiffuseProduct");
	sp_location = glGetUniformLocation(program, "SpecularProduct");
	light_position_location = glGetUniformLocation(program, "LightPosition");

	ac_location = glGetUniformLocation(program, "attenuation_constant");
	al_location = glGetUniformLocation(program, "attenuation_linear");
	aq_location = glGetUniformLocation(program, "attenuation_quadratic");

	shininess_location = glGetUniformLocation(program, "shininess");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glDepthRange(1, 0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform4fv(light_position_location, 1, (GLfloat *)&light_position);

	glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *)&mv_matrix);
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *)&projection);
	
	glUniform1fv(ac_location, 1, (GLfloat *)&attenuation_constant);
	glUniform1fv(al_location, 1, (GLfloat *)&attenuation_linear);
	glUniform1fv(aq_location, 1, (GLfloat *)&attenuation_quadratic);

	// Table
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&table_ctm);
	AmbientProduct = product(table_material.reflect_ambient, light_ambient);
	DiffuseProduct = product(table_material.reflect_diffuse, light_diffuse);
	SpecularProduct = product(table_material.reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&table_material.shininess);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	// Red Sphere
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&red_sphere_ctm);
	AmbientProduct = product(ball_materials[0].reflect_ambient, light_ambient);
	DiffuseProduct = product(ball_materials[0].reflect_diffuse, light_diffuse);
	SpecularProduct = product(ball_materials[0].reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&ball_materials[0].shininess);
	glDrawArrays(GL_TRIANGLES, 36, num_sphere_vertices);

	glUniform1i(isShadow_location, 1);
	glDrawArrays(GL_TRIANGLES, 36, num_sphere_vertices);

	// Green Sphere
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&green_sphere_ctm);
	AmbientProduct = product(ball_materials[1].reflect_ambient, light_ambient);
	DiffuseProduct = product(ball_materials[1].reflect_diffuse, light_diffuse);
	SpecularProduct = product(ball_materials[1].reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&ball_materials[1].shininess);
	glDrawArrays(GL_TRIANGLES, 14730, num_sphere_vertices);

	glUniform1i(isShadow_location, 1);
	glDrawArrays(GL_TRIANGLES, 14730, num_sphere_vertices);

	// Blue Sphere
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&blue_sphere_ctm);
	AmbientProduct = product(ball_materials[2].reflect_ambient, light_ambient);
	DiffuseProduct = product(ball_materials[2].reflect_diffuse, light_diffuse);
	SpecularProduct = product(ball_materials[2].reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&ball_materials[2].shininess);
	glDrawArrays(GL_TRIANGLES, 29424, num_sphere_vertices);

	glUniform1i(isShadow_location, 1);
	glDrawArrays(GL_TRIANGLES, 29424, num_sphere_vertices);

	// Yellow Sphere
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&yellow_sphere_ctm);
	AmbientProduct = product(ball_materials[3].reflect_ambient, light_ambient);
	DiffuseProduct = product(ball_materials[3].reflect_diffuse, light_diffuse);
	SpecularProduct = product(ball_materials[3].reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&ball_materials[3].shininess);
	glDrawArrays(GL_TRIANGLES, 44118, num_sphere_vertices);

	glUniform1i(isShadow_location, 1);
	glDrawArrays(GL_TRIANGLES, 44118, num_sphere_vertices);

	// Orange Sphere
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&orange_sphere_ctm);
	AmbientProduct = product(ball_materials[4].reflect_ambient, light_ambient);
	DiffuseProduct = product(ball_materials[4].reflect_diffuse, light_diffuse);
	SpecularProduct = product(ball_materials[4].reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&ball_materials[4].shininess);
	glDrawArrays(GL_TRIANGLES, 58812, num_sphere_vertices);

	glUniform1i(isShadow_location, 1);
	glDrawArrays(GL_TRIANGLES, 58812, num_sphere_vertices);

	// Light Sphere
	glUniform1i(isShadow_location, 0);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *)&white_sphere_ctm);
	AmbientProduct = product(ball_materials[5].reflect_ambient, light_ambient);
	DiffuseProduct = product(ball_materials[5].reflect_diffuse, light_diffuse);
	SpecularProduct = product(ball_materials[5].reflect_specular, light_specular);
	glUniform4fv(ap_location, 1, (GLfloat *)&AmbientProduct);
	glUniform4fv(dp_location, 1, (GLfloat *)&DiffuseProduct);
	glUniform4fv(sp_location, 1, (GLfloat *)&SpecularProduct);
	glUniform1fv(shininess_location, 1, (GLfloat *)&ball_materials[5].shininess);
	glDrawArrays(GL_TRIANGLES, 73506, num_sphere_vertices);
	
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
	// Undo translation of the light
	mat4 neg_light_tr_matrix = translate(-light_x, -light_y, -light_z);

	// Add these checks to see what is being modified
	int eye_change = 0;
	int light_change = 0;

	// Terminate program
	if (key == 'q') {
		exit(0);
	}

	// Eye controls, moves eye point using parametric equation of a sphere
	// Adjust the theta value
	else if (key == 't') {
		theta -= 1.0f;
		eye_change = 1;
	}
	else if (key == 'T') {
		theta += 1.0f;
		eye_change = 1;
	}
	// Adjust the phi value
	else if (key == 'p') {
		phi -= 1.0f;
		eye_change = 1;
	}
	else if (key == 'P') {
		phi += 1.0f;
		eye_change = 1;
	}
	// Adjust the radius
	else if (key == 'r') {
		radius -= 0.1f;
		eye_change = 1;
	}
	else if (key == 'R') {
		radius += 0.1f;
		eye_change = 1;
	}

	// Light controls, moves light along x, y, z coordinate system
	else if (key == 'x') {
		light_x -= 0.1f;
		light_change = 1;
	}
	else if (key == 'X') {
		light_x += 0.1f;
		light_change = 1;
	}
	else if (key == 'y') {
		light_y -= 0.1f;
		light_change = 1;
	}
	else if (key == 'Y') {
		light_y += 0.1f;
		light_change = 1;
	}
	else if (key == 'z') {
		light_z -= 0.1f;
		light_change = 1;
	}
	else if (key == 'Z') {
		light_z += 0.1f;
		light_change = 1;
	}

	// Enable animation
	// Press space bar start/stop animation
	else if (key == ' ' && enableIdle == 0) {
		enableIdle = 1;
	}
	else if (key == ' ' && enableIdle == 1) {
		enableIdle = 0;
	}

	// If the user adjust the eye point, update the model view matrix with the new eye point
	if (eye_change == 1) {
		vec4 eye = calculateEyePoint(theta, phi, radius);
		eye_x = eye.x;
		eye_y = eye.y;
		eye_z = eye.z;
		mv_matrix = look_at(eye_x, eye_y, eye_z, at_x, at_y, at_z, 0.0, 1.0, 0.0);
	}
	
	// If the user adjusts the light position, adjust the current light position with the new one 
	if (light_change == 1) {
		mat4 light_tr_matrix = translate(light_x, light_y, light_z);
		white_sphere_ctm = m4_mult(neg_light_tr_matrix, white_sphere_ctm);
		white_sphere_ctm = m4_mult(light_tr_matrix, white_sphere_ctm);

		vec4 new_light_position = { light_x, light_y, light_z, 1.0 };
		light_position = new_light_position;
	}
	glutPostRedisplay();
}

void idle(void)
{
	if (enableIdle)
	{
		// Calculate the points on a circle using parametric equation of a circle
		GLfloat x = sin(green_angle * DegreesToRadians);
		GLfloat z = cos(green_angle * DegreesToRadians);

		// Translate the sphere to its next position
		green_sphere_ctm = m4_identity();
		green_sphere_ctm = translate(x - 1, 0.0f, z);

		x = 2.0f * sin(blue_angle * DegreesToRadians);
		z = 2.0f * cos(blue_angle * DegreesToRadians);

		blue_sphere_ctm = m4_identity();
		blue_sphere_ctm = translate(x - 2, 0.0f, z);

		x = 3.0f * sin(yellow_angle * DegreesToRadians);
		z = 3.0f * cos(yellow_angle * DegreesToRadians);

		yellow_sphere_ctm = m4_identity();
		yellow_sphere_ctm = translate(x - 3, 0.0f, z);

		x = 4.0f * sin(orange_angle * DegreesToRadians);
		z = 4.0f * cos(orange_angle * DegreesToRadians);

		orange_sphere_ctm = m4_identity();
		orange_sphere_ctm = translate(x - 4, 0.0f, z);

		// Increment the current angle by their rotation speed/angles
		green_angle += green_rotation_speed;
		blue_angle += blue_rotation_speed;
		yellow_angle += yellow_rotation_speed;
		orange_angle += orange_rotation_speed;

		// If the angle is greater than 360 degrees, reset the angle counter back to 0 degrees
		if (green_angle > 360.0f) {
			green_angle = 0.0f;
		}
		if (blue_angle > 360.0f) {
			blue_angle = 0.0f;
		}
		if (yellow_angle > 360.0f) {
			yellow_angle = 0.0f;
		}
		if (orange_angle > 360.0f) {
			orange_angle = 0.0f;
		}

		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(window_position_width, window_position_height);
	glutCreateWindow("Project 3: Lighting Effects and Shadows by Jason Ly");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}

// Function used to zoom in and out
mat4 scale(float sx, float sy, float sz) {
	mat4 result;
	result.x.x = sx; result.y.x = 0;  result.z.x = 0;  result.w.x = 0;
	result.x.y = 0;  result.y.y = sy; result.z.y = 0;  result.w.y = 0;
	result.x.z = 0;	 result.y.z = 0;  result.z.z = sz; result.w.z = 0;
	result.x.w = 0;	 result.y.w = 0;  result.z.w = 0;  result.w.w = 1;
	return result;
}

// Function used to move an object in the world
mat4 translate(float dx, float dy, float dz) {
	mat4 result;
	result.x.x = 1; result.y.x = 0; result.z.x = 0;  result.w.x = dx;
	result.x.y = 0; result.y.y = 1; result.z.y = 0;  result.w.y = dy;
	result.x.z = 0;	result.y.z = 0; result.z.z = 1;  result.w.z = dz;
	result.x.w = 0;	result.y.w = 0; result.z.w = 0;  result.w.w = 1;
	return result;
}

mat4 look_at(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat atx, GLfloat aty, GLfloat atz, GLfloat upx, GLfloat upy, GLfloat upz) {
	vec4 e = { eyex, eyey, eyez, 0 };
	vec4 a = { atx, aty, atz, 0 };
	vec4 vup = { upx, upy, upz, 0 };

	vec4 vpn = v4_sub(e, a);
	vec4 n = v4_unit_vec(vpn);
	vec4 u = v4_unit_vec(v4_cross_prod(vup, n));
	vec4 v = v4_unit_vec(v4_cross_prod(n, u));
	vec4 p = { eyex, eyey, eyez, 1 };

	mat4 m;
	m.x.x = u.x; m.y.x = u.y; m.z.x = u.z; m.w.x = u.w;
	m.x.y = v.x; m.y.y = v.y; m.z.y = v.z; m.w.y = v.w;
	m.x.z = n.x; m.y.z = n.y; m.z.z = n.z; m.w.z = n.w;
	m.x.w = p.x; m.y.w = p.y; m.z.w = p.z; m.w.w = p.w;

	mat4 m_transpose = m4_transpose(m);
	mat4 m_transpose_inverse = m4_inverse(m_transpose);

	return m_transpose_inverse;
}

mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val) {
	mat4 result;
	result.x.x = -(near_val / right); result.y.x = 0;				  result.z.x = 0;											result.w.x = 0;
	result.x.y = 0;					  result.y.y = -(near_val / top); result.z.y = 0;										    result.w.y = 0;
	result.x.z = 0;					  result.y.z = 0;				  result.z.z = (near_val + far_val) / (far_val - near_val); result.w.z = -(2 * near_val * far_val) / (far_val - near_val);
	result.x.w = 0;					  result.y.w = 0;				  result.z.w = -1.0f;									    result.w.w = 0;
	return result;
}

// Calculates the eye point using parametric equations of a sphere
vec4 calculateEyePoint(GLfloat theta, GLfloat phi, GLfloat radius) {
	vec4 eye;
	GLfloat theta_radian = theta*DegreesToRadians;
	GLfloat phi_radian = phi*DegreesToRadians;
	eye.x = radius*cos(theta_radian)*sin(phi_radian);
	eye.y = radius*cos(phi_radian);
	eye.z = radius*sin(theta_radian)*sin(phi_radian); 
	eye.w = 1.0;
	return eye;
}

vec4 product(vec4 u, vec4 v)
{
	vec4 result;
	result.x = u.x * v.x;
	result.y = u.y * v.y;
	result.z = u.z * v.z;
	result.w = u.w * v.w;
	return result;
}
