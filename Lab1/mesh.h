#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"
//Tree points to make a triangle. In order to reuse a point. Because storing points cost resources. All the points stored in one array, 
//and when we want to make the triangle we take this ints, indexes to another array. 3 int 3 indexes.
typedef struct _Triangle {
	int vInds[3]; //vertex indices
} Triangle;
//Structure for our meshes, mesh = set of triangles  
typedef struct _Mesh { 
	int nv;	//Number of vetex			
	Vector *vertices; //Array of vectors, not homogenous. Real 3d points
	Vector *vnorms; //Normalized vectors, vertex normals, unit vector. Points outwards from an object. Every vertex has a normal, nv = size of this.
	int nt;	//Number of triangles.
	Triangle *triangles; // Struct
	struct _Mesh *next;  //Pointer to next mesh.
	Vector translation;
	Vector scaling;
	Vector rotation;

	unsigned int vbo, ibo, vao; // OpenGL handles for rendering, Read about them. Handles basically.
	//You can not render it if its already in the memory. It needs to be in the graphiccard memory. Therefore it gives you int to handles that later refere to you vertexs indexes etc.
} Mesh;

typedef struct _Camera {
	Vector position;
	Vector rotation;
	double fov; //Field of view, in x-axis, left right. It can also be fov y. top and bottom.
	double nearPlane; //Two more planes. 
	double farPlane; 
} Camera;

typedef struct View {
	bool OrthoProj;
	bool FOV;
} CameraView;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale = 1.0, Vector translation = { 0,0,0 }, Vector scaling = { 1,1,1 }, Vector rotation = { 0,0,0 }); //For the meshes. 

#endif
