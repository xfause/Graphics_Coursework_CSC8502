#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class Camera	{
public:
	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
		speed = 1.0f;
	};

	Camera(float pitch, float yaw, Vector3 position, float speed) {
		this->pitch		= pitch;
		this->yaw		= yaw;
		this->position	= position;
		this->speed = speed;
	}

	~Camera(void){};

	void UpdateCamera(float dt = 1.0f);

	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	Matrix4 BuildViewMatrix();

	//Gets position in world space
	Vector3 GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

protected:
	float	yaw;
	float	pitch;
	Vector3 position;
	float   speed;
};