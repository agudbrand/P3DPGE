#pragma once
#include "System.h"

struct Mesh;
struct Vector3;
struct Matrix3;
struct Matrix4;
struct Entity;
struct Transform;

struct MeshSystem : public System {
	//function description goes here
	static Mesh* BuildBoxMesh(Entity* e, Transform* t, Vector3 halfDimensions);

	//function description goes here
	static Mesh* BuildComplexMesh(Entity* e, Transform* t, const char* fileName, bool hasTexture);

	//function description goes here
	static void TranslateMesh(Mesh* mesh, Vector3 translation);

	//function description goes here
	static void RotateMesh(Mesh* mesh, Matrix4 rotation);

	//function description goes here
	static void ScaleMesh(Mesh* mesh, Matrix3 scale);

	//function description goes here
	static void TransformMesh(Mesh* mesh, Matrix4 transformation);

	void Update() override;
};