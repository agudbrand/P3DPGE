#pragma once
#include "System.h"

struct Mesh;
struct Vector3;
struct Matrix3;
struct Matrix4;
struct Entity;
struct Transform;
struct Edge3D;

struct MeshSystem : public System {
	void Init() override;
	void Update() override;

	//returns true if a line intersects with one of a mesh's triangles
	static bool LineIntersect(Mesh* mesh, Edge3D* line);

	//function description goes here
	static void TranslateMesh(Mesh* mesh, Vector3 translation);

	//function description goes here
	static void RotateMesh(Mesh* mesh, Matrix4 rotation);

	//function description goes here
	static void ScaleMesh(Mesh* mesh, Matrix3 scale);

	//function description goes here
	static void TransformMesh(Mesh* mesh, Matrix4 transformation);
};