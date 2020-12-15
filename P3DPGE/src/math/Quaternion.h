#pragma once
#include "UsefulMathDefines.h"

struct Vector3;

//TODO(,delle) implement quaternions
struct Quaternion {
	float x{}, y{}, z{}, w{};

	Quaternion() : x(0), y(0), z(0), w(1) {}
	Quaternion(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}

	//Non-Quat vs Quat interactions defined in Math.h
	Quaternion(const Vector3& rotation);
	Quaternion(const Vector3& axis, float theta);
};