#pragma once
#include "vector3.h"

class Plane {
public:
	Plane(void) {}
	Plane(const Vector3& n, float distance, bool normalise = false);
	~Plane(void) {}

	void SetNormal(const Vector3& n) { this->normal = n; }
	Vector3 GetNormal() const { return normal; }

	void SetDistance(float d) { distance = d; }
	float GetDistance() const { return distance; }

	bool SphereInPlane(const Vector3& postion, float radius) const;

protected:
	Vector3 normal;
	float distance;
};