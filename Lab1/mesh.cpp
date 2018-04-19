#include <stdlib.h>
#include "mesh.h"
#include <stdio.h>

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale, Vector translation, Vector scaling, Vector rotation) {
	Mesh * mesh = (Mesh *) malloc(sizeof(Mesh));
	mesh->nv = nv;
	mesh->nt = nt;	
	mesh->vertices = (Vector *) malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)malloc(nv * sizeof(Vector));
	mesh->triangles = (Triangle *) malloc(nt * sizeof(Triangle));
	mesh->translation = { 0, 0, 0};
	mesh->scaling = { 1, 1, 1};
	mesh->rotation = { 0, 0, 0 };
	

	// set mesh vertices
	for (int i = 0; i < nv; i++) {
		mesh->vertices[i].x = vArr[i*3] * scale;
		mesh->vertices[i].y = vArr[i*3+1] * scale;
		mesh->vertices[i].z = vArr[i*3+2] * scale;
	}

	// set mesh triangles
	for (int i = 0; i < nt; i++) {
		mesh->triangles[i].vInds[0] = tArr[i*3]; //Från triangel till index. 
		mesh->triangles[i].vInds[1] = tArr[i*3+1];
		mesh->triangles[i].vInds[2] = tArr[i*3+2];

		const int ia = mesh->triangles[i].vInds[0];
		const int ib = mesh->triangles[i].vInds[1];
		const int ic = mesh->triangles[i].vInds[2];
		//Räkna ut vektor mellan 0 - 1, edges 
		// Räkna ut vektor mellan 0 - 1, edge2
		Vector e1 = Subtract(mesh->vertices[ib], mesh->vertices[ia]); //Vector b - a 
		Vector e2 = Subtract(mesh->vertices[ic], mesh->vertices[ia]); //Vector c - a
		Vector no = Normalize(CrossProduct(e2, e1)); // En vektor som är vinkelrät från e1 och e2 i punkt 0. 
		mesh->vnorms[ia] = Add(mesh->vnorms[ia], no); // Vi tar 0's normal och adderar till no då det kan vara så att 0 har en annan punkt hos andra trianglar.
		mesh->vnorms[ib] = Add(mesh->vnorms[ib], no);
		mesh->vnorms[ic] = Add(mesh->vnorms[ic], no);
		//Vi räknar ut mitt punktens normal därför måste vi addera två st för att få punktens normal. 
		//mesh->vnorms är en punkt i triangeln och no är det som pekar ut från mitten av triangeln. man får den riktiga normalen.
		//Calculate facenormals. 
	}

	// Assignment 1: 
	// Calculate and store suitable vertex normals for the mesh here.
	// Replace the code below that simply sets some arbitrary normal values	
	//Calculates surface normals of points in triangles.
	for (int i = 0; i < nv; i++) 
	{
		mesh->vnorms[i] = Normalize(mesh->vnorms[i]); // Eftersom resultatet innan gett många ihop satta resultat därför så normaliserar vi vektorn och gör den till 1. Normalisera varje punkt.
	}



	mesh->next = *list;
	*list = mesh;	
}
