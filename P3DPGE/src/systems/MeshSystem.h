#pragma once
#include "System.h"

struct Mesh;
struct Vector3;
struct Matrix3;
struct Matrix4;
struct Entity;

struct MeshSystem : public System {
	//function description goes here
	Mesh* BuildBoxMesh(Entity* e, Vector3 origin, Vector3 halfDimensions);

	//function description goes here
	Mesh* BuildComplexMesh(Entity* e, Vector3 origin, const char* fileName, bool hasTexture);

	//function description goes here
	void TranslateMesh(Mesh* mesh, Vector3 translation);

	//function description goes here
	void RotateMesh(Mesh* mesh, Matrix4 rotation);

	//function description goes here
	void ScaleMesh(Mesh* mesh, Matrix3 scale);

	//function description goes here
	void TransformMesh(Mesh* mesh, Matrix4 transformation);

	//function description goes here
	void Update(float deltaTime, olc::PixelGameEngine* p) override;
};