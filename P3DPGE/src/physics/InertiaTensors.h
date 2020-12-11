#pragma once
#include "../math/Math.h"

namespace InertiaTensors {
	static Matrix SolidSphere(float radius, float mass) {
		float value = .4f * mass * radius * radius;
		return Matrix(3, 3, {
			value, 0, 0,
			0, value, 0,
			0, 0, value
			});
	}

	static Matrix HollowSphere(float radius, float mass) {
		float value = M_TWOTHIRDS * mass * radius * radius;
		return Matrix(3, 3, {
			value, 0, 0,
			0, value, 0,
			0, 0, value
			});
	}

	static Matrix SolidEllipsoid(Vector3 halfDimensions, float mass) {
		float oneFifthMass = .2f * mass;
		float aSqrd = halfDimensions.x * halfDimensions.x;
		float bSqrd = halfDimensions.y * halfDimensions.y;
		float cSqrd = halfDimensions.z * halfDimensions.z;
		return Matrix(3, 3, {
			oneFifthMass* (bSqrd + cSqrd), 0, 0,
			0, oneFifthMass* (bSqrd + cSqrd), 0,
			0, 0, oneFifthMass* (bSqrd + cSqrd)
			});
	}

	static Matrix SolidCuboid(float width, float height, float depth, float mass) {
		float oneTwelfthMass = M_ONETWELFTH * mass;
		float wSqrd = width * width;
		float hSqrd = height * height;
		float dSqrd = depth * depth;
		return Matrix(3, 3, {
			oneTwelfthMass* (hSqrd + dSqrd), 0, 0,
			0, oneTwelfthMass* (wSqrd + dSqrd), 0,
			0, 0, oneTwelfthMass* (wSqrd + hSqrd)
			});
	}

	static Matrix SolidCylinder(float radius, float height, float mass) {
		float rSqrd = radius * radius;
		float value = M_ONETWELFTH * mass * (3 * rSqrd + height * height);
		return Matrix(3, 3, {
			value, 0, 0,
			0, value, 0,
			0, 0, .5f * mass * rSqrd
			});
	}
};