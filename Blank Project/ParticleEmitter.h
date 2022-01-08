#pragma once
#include "../NCLGL/Vector3.h"
#include "../NCLGL/Vector4.h"
#include "../NCLGL/OGLRenderer.h"
#include "../NCLGL/Mesh.h"
#include <vector>

struct Particle {
	Vector3 position;
	Vector4 colour;

	float	life;
	float	fade;					// Fade Speed
	float	r;						// Red Value
	float	g;						// Green Value
	float	b;						// Blue Value
	float	x;						// X Position
	float	y;						// Y Position
	float	z;						// Z Position
	float	xi;						// X Direction
	float	yi;						// Y Direction
	float	zi;						// Z Direction
	float	xg;						// X Gravity
	float	yg;						// Y Gravity
	float	zg;						// Z Gravity
};

class ParticleEmitter : public Mesh {
public:
	ParticleEmitter(void);
	~ParticleEmitter(void);
	void Update(float msec);

	virtual void Draw();
	float	GetParticleSize() { return particleSize; }
	void	SetParticleSize(float size) { particleSize = size; }

protected:
	Particle* GetNewParticle();
	void	ResizeArrays();

	GLuint texture;
	float particleSize;

	float xspeed = 0.0f;	
	float yspeed = 0.0f;

	float zoom = -40.0f;
	float posX = 0.0f;
	float posY = 0.0f;
	float posZ = 0.0f;

	float gravX = 0.0f;
	float gravY = 0.0f;
	float gravZ = 0.0f;

	const GLfloat white[3] = { 1.0f,1.0f,1.0f };
	const GLfloat blue[3] = { 0.0f,0.0f,1.0f };
	const GLfloat yellow[3] = { 1.0f,1.0f,0.0f };
	const GLfloat orange[3] = { 1.0f,0.5f,0.0f };
	const GLfloat red[3] = { 1.0f,0.1f,0.0f };

	int PARTICLE_NUMS = 10000.0f;
	std::vector<Particle*>	particles;	//Active particles stay in here :)
};

