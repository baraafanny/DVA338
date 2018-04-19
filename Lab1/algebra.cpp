#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
#include "algebra.h"

#define PI 3.14159265

double cotan(double i) { return(1 / tan(i * PI / 180.0)); }
Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

float DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}

Matrix T(Vector position)
{
	Matrix a;

	a.e[0] = 1; a.e[4] = 0; a.e[8] = 0; a.e[12] = position.x;
	a.e[1] = 0; a.e[5] = 1; a.e[9] = 0; a.e[13] = position.y;
	a.e[2] = 0; a.e[6] = 0; a.e[10] = 1; a.e[14] = position.z;
	a.e[3] = 0; a.e[7] = 0; a.e[11] = 0; a.e[15] = 1;

	return a;
}
Matrix S(Vector position)
{
	Matrix a;

	a.e[0] = position.x; a.e[4] = 0; a.e[8] = 0; a.e[12] = 0;
	a.e[1] = 0; a.e[5] = position.y; a.e[9] = 0; a.e[13] = 0;
	a.e[2] = 0; a.e[6] = 0; a.e[10] = position.z; a.e[14] = 0;
	a.e[3] = 0; a.e[7] = 0; a.e[11] = 0; a.e[15] = 1;

	return a;
}
Matrix Rx(float param)
{
	Matrix a;

	a.e[0] = 1; a.e[4] = 0; a.e[8] = 0; a.e[12] = 0;
	a.e[1] = 0; a.e[5] = cos(param * PI / 180.0); a.e[9] = (-1)*(sin(param * PI / 180.0)); a.e[13] = 0;
	a.e[2] = 0; a.e[6] = sin(param * PI / 180.0); a.e[10] = cos(param * PI / 180.0); a.e[14] = 0;
	a.e[3] = 0; a.e[7] = 0; a.e[11] = 0; a.e[15] = 1;

	return a;
}
Matrix Ry(float param)
{
	Matrix a;

	a.e[0] = cos(param * PI / 180.0); a.e[4] = 0; a.e[8] = sin(param * PI / 180.0); a.e[12] = 0;
	a.e[1] = 0; a.e[5] = 1; a.e[9] = 0; a.e[13] = 0;
	a.e[2] = (-1)*(sin(param * PI / 180.0)); a.e[6] = 0; a.e[10] = cos(param * PI / 180.0); a.e[14] = 0;
	a.e[3] = 0; a.e[7] = 0; a.e[11] = 0; a.e[15] = 1;

	return a;
}
Matrix Rz(float param)
{
	Matrix a;

	a.e[0] = cos(param * PI / 180.0); a.e[4] = (-1)*(sin(param * PI / 180.0)); a.e[8] = 0; a.e[12] = 0;
	a.e[1] = sin(param * PI / 180.0); a.e[5] = cos(param * PI / 180.0); a.e[9] = 0; a.e[13] = 0;
	a.e[2] = 0; a.e[6] = 0; a.e[10] = 1; a.e[14] = 0;
	a.e[3] = 0; a.e[7] = 0; a.e[11] = 0; a.e[15] = 1;

	return a;
}
Matrix matOrtho(double left, double right, double bottom, double top, double near, double far)//ORTHOGONAL PROJECTION TRANSFORM
{
	Matrix a;

	a.e[0] = 2/(right - left); a.e[4] = 0; a.e[8] = 0; a.e[12] = (-1)*((right + left) / (right - left));
	a.e[1] = 0; a.e[5] = 2/(top - bottom); a.e[9] = 0; a.e[13] = (-1)*((top + bottom) / (top - bottom));
	a.e[2] = 0; a.e[6] = 0; a.e[10] = 2/(near - far); a.e[14] = (-1)*((far + near) / (far - near));
	a.e[3] = 0; a.e[7] = 0; a.e[11] = 0; a.e[15] = 1;

	return a;
}
Matrix matPerpective(double fovy, double aspect, double near, double far) //FOV PERSPECTIVE
{
	Matrix a;

	a.e[0] = (cotan(fovy/2))/aspect; a.e[4] = 0; a.e[8] = 0; a.e[12] = 0;
	a.e[1] = 0; a.e[5] = cotan(fovy/2); a.e[9] = 0; a.e[13] = 0;
	a.e[2] = 0; a.e[6] = 0; a.e[10] = (far+near)/(near-far); a.e[14] = (2 * far * near)/(near-far);
	a.e[3] = 0; a.e[7] = 0; a.e[11] = -1; a.e[15] = 0;

	return a;
}


void PrintVector(char *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}
