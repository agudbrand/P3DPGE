#pragma once
#include "System.h"

struct OldMesh;
struct Vector3;
struct Matrix3;
struct Matrix4;
struct Entity;
struct Transform;
struct Edge3D;

struct MeshSystem : public System {
	void Init() override;
	void Update() override;

	//for using LookAt matrix to rotate
	static const bool USE_LOOKAT = true;

	//returns true if a line intersects with one of a mesh's triangles
	static bool LineIntersect(OldMesh* mesh, Edge3D* line);

	//function description goes here
	static void TranslateMesh(OldMesh* mesh, Vector3 translation);

	//function description goes here
	static void RotateMesh(OldMesh* mesh, Matrix4 rotation);

	//function description goes here
	static void ScaleMesh(OldMesh* mesh, Matrix3 scale);

	//function description goes here
	static void TransformMesh(OldMesh* mesh, Matrix4 transformation);
};