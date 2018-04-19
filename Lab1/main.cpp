//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>

#include "algebra.h"
#include "shaders.h"
#include "mesh.h"


int screen_width = 1024;
int screen_height = 768;

Mesh *meshList = NULL; // Global pointer to linked list of triangle meshes

Camera cam = {{0,0,20}, {0,0,0}, 60, 1, 10000}; // Setup the global camera parameters
//Depth that we are looking at in OpenGL is negative. stepping back is positive, front negative. 
//60 fov, human field of view.

CameraView View{ false, true }; // For user viewing.

GLuint shprg; // Shader program id

// Global transform matrices
// V is the view transform
// P is the projection transform
// PV = P * V is the combined view-projection transform
Matrix V, P, PV;

void prepareShaderProgram(const char ** vs_src, const char ** fs_src) {
	GLint success = GL_FALSE;
	
	shprg = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void prepareMesh(Mesh *mesh) {
	
	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols  = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);
	
	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 *sizeof(float)));

	glBindVertexArray(0);

}

void renderMesh(Mesh *mesh) {

	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below
	// set mesh vertices
	Matrix W;
	W = MatMatMul(Rz(mesh->rotation.z), S(mesh->scaling));
	W = MatMatMul(Ry(mesh->rotation.y), W);
	W = MatMatMul(Rx(mesh->rotation.x), W);
	W = MatMatMul(T(mesh->translation), W);
	//Matrix M = MatMatMul(V, W);
	
	Matrix PVW = MatMatMul(PV, W);
	// Pass the viewing transform to the shader
	GLint loc_PVW = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PVW, 1, GL_FALSE, PVW.e);

	// Select current resources 
	glBindVertexArray(mesh->vao);
	
	//Enables culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	// Compile and link the given shader program (vertex shader and fragment shader)

	//Tell GLUT that a depth buffer is needed, z-buffer.
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	// Reset the depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);	//change the way depths values are compared with each other
	glDepthFunc(GL_LESS);

	// To accomplish wireframe rendering (can be removed to get filled triangles)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL); 
	glBindVertexArray(0);
	
}

//R = x G = y B = z

void display(void) {
	Mesh *mesh;
	
	glClear(GL_COLOR_BUFFER_BIT);	
	// Assignment 1: Calculate the transform to view coordinates yourself 	
	// The matrix V should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 
	V = MatMatMul(Rx((-1*cam.rotation.x)), T(ScalarVecMul(-1.0, cam.position)));
	V = MatMatMul(Ry((-1)*cam.rotation.y), V);
	V = MatMatMul(Rz((-1)*cam.rotation.z), V);

	//V = MatMatMul((MatMatMul(Rx((-1)*cam.rotation.x), Ry((-1)*cam.rotation.y))), (MatMatMul(Rz((-1)*cam.rotation.z), T(ScalarVecMul(-1.0, cam.position)))));
	PrintMatrix("V", V);
	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 	
	if (View.OrthoProj == true)
	{
		double OrthoW = screen_width / 2;
		double OrthoH = screen_height / 2;

		P = matOrtho(-OrthoW/5, OrthoW/5, -OrthoH/5, OrthoH/5, cam.nearPlane, cam.farPlane);
	}

	if (View.FOV == true)
	{
		P = matPerpective(cam.fov, (screen_width / screen_height), cam.nearPlane, cam.farPlane);
	}
	PrintMatrix("P", P);

	// This finds the combined view-projection matrix
	PV = MatMatMul(P, V);
	//Put object itself in the world, just for the view and projection, which is same for everyone. 
	//Everyobject i need a separate matrix. 

	// Select the shader program to be used during rendering 
	glUseProgram(shprg);

	// Render all meshes in the scene
	mesh = meshList;
		
	while (mesh != NULL) {

		renderMesh(mesh);
		mesh = mesh->next;
	}

	glFlush();
}

void changeSize(int w, int h) {
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);

}

void keypress(unsigned char key, int x, int y)
{
	switch(key) {
	case 'x':
		cam.position.x -= 0.5;
		break;
	case 'X':
		cam.position.x += 0.5;
		break;
	case 'y':
		cam.position.y -= 0.5;
		break;
	case 'Y':
		cam.position.y += 0.5;
		break;
	case 'z':
		cam.position.z -= 0.5;
		break;
	case 'Z':
		cam.position.z += 0.5;
		break;
	case 'i':
		cam.rotation.x -= 0.5;
		break;
	case 'I':
		cam.rotation.x += 0.5;
		break;
	case 'j':
		cam.rotation.y -= 0.5;
		break;
	case 'J':
		cam.rotation.y += 0.5;
		break;
	case 'k':
		cam.rotation.z -= 0.5;
		break;
	case 'K':
		cam.rotation.z += 0.5;
		break;
	case 'p':
		View.FOV = true;
		View.OrthoProj = false;
		break;
	case 'o':
		View.OrthoProj = true;
		View.FOV = false;
		break;
	case 's':
		meshList->scaling.x -= 0.2;
		break;
	case 'S':
		meshList->scaling.x += 0.2;
		break;
	case 't':
		meshList->translation.x -= 0.2;
		break;
	case 'T':
		meshList->translation.x += 0.2;
		break;
	case 'r':
		meshList->rotation.z -= 0.2;
		break;
	case 'R':
		meshList->rotation.z += 0.2;
		break;
	case 'Q':
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void init(void) {

	prepareShaderProgram(vs_n2c_src, fs_ci_src);

	// Setup OpenGL buffers for rendering of the meshes
	Mesh * mesh = meshList;
	while (mesh != NULL) {
		prepareMesh(mesh);
		mesh = mesh->next;
	}	
}

void cleanUp(void) {	
	printf("Running cleanUp function... ");
	// Free openGL resources
	// ...
	free(meshList);
	
	// Free meshes
	// ...
	printf("Done!\n\n");
}

// Include data for some triangle meshes (hard coded in struct variables)
#include "./models/mesh_bunny.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"

//Dont touchy
//OpenGL for rendering, Glut knows about ur OS and handles window management, user management, drawing stuff on the screen.

int main(int argc, char **argv) {
	// Setup freeGLUT	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display); //Calls this func at every frame.
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Specify your preferred OpenGL version and profile
	glutInitContextVersion(4, 5);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);	
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Uses GLEW as OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit(); 
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list
	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	//insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	//insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);	
	//insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 1.0, { -10,-5,-2 }, { 1, 1, 1 }, { 0,0, 0 });
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);
	//insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);
	
	init();
	glutMainLoop();
	cleanUp();	
	return 0;
}
