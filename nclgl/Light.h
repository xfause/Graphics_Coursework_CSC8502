#pragma once

#include "Vector3.h"
#include "Vector4.h"

class Light {
public:
	Light() {}
	Light(const Vector3& position, const Vector4& colour, float radius) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}
	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float f) { radius = f; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

protected:
	Vector3 position;
	float radius;
	Vector4 colour;
};

class DirectionLight {
public:
	DirectionLight() {}
	DirectionLight(const Vector3& direction, const Vector4& colour) {
		this->direction = direction;
		this->colour = colour;
	}
	~DirectionLight(void) {};

	Vector3 GetDirection() const { return direction; }
	void SetDirection(const Vector3& val) { direction = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

protected:
	Vector3 direction;
	Vector4 colour;
};

class SpotLight {
public:
	SpotLight() {}
	SpotLight(const Vector3& position, const Vector3& direction, const Vector4& colour, float cutoff, float outCutoff) {
		this->position = position;
		this->direction = direction;
		this->colour = colour;
		this->cutoff = cutoff;
		this->outCutoff = outCutoff;
	}
	~SpotLight(void) {};

	Vector3 GetDirection() const { return direction; }
	void SetDirection(const Vector3& val) { direction = val; }

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

	float GetCutoff() const { return cutoff; }
	void SetCutoff(const float val) { cutoff = val; }

	float GetOutCutoff() const { return outCutoff; }
	void SetOutCutoff(const float val) { outCutoff = val; }

protected:
	Vector3 position;
	Vector3 direction;
	Vector4 colour;
	float cutoff;
	float outCutoff;
};