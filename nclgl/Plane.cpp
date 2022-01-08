#include "Plane.h"

Plane::Plane(const Vector3& normal, float distance, bool normalise) {
	if (normalise) {
		float length = sqrt(Vector3::Dot(normal, normal));
		this->normal = normal / length;
		this->distance = distance / length;
	}
	else {
		this->normal = normal;
		this->distance = distance;
	}
}

bool Plane::SphereInPlane(const Vector3& position, float radius) const {
	// normal point to Frustum inside
	// so if dot(n,p) + d > 0 means inside the frustum

	if (Vector3::Dot(position, normal) + distance <= -radius) {
		return false;
	}
	return true;
}