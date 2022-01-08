#include "ParticleEmitter.h"
#include <string>
#include <io.h>
#include <stdio.h>

ParticleEmitter::ParticleEmitter(void) {
	for (int i = 0; i < PARTICLE_NUMS; ++i) {
		particles.push_back(GetNewParticle());
	}
	ResizeArrays();

	texture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/flame_particle.bmp",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
}

ParticleEmitter::~ParticleEmitter(void) {
	for (std::vector<Particle*>::iterator i = particles.begin(); i != particles.end(); ++i) {
		delete (*i);
	}
	glDeleteTextures(1, &texture);
}
void ParticleEmitter::Update(float msec) {
	
	for (std::vector<Particle*>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */) {
		Particle* p = (*i);
		p->colour = Vector4(p->r, p->g, p->b, p->life);
		p->position = Vector3(p->x,p->y,p->z + zoom);

		p->x += p->xi / (750);
		p->y += p->yi / (1000);
		p->z += p->zi / (750);

		if ((p->x > posX) && (p->y > (0.1 + posY))) {
			p->xg = -0.3f;
		}
		else if ((p->x < posX) && (p->y > (0.1 + posY))) {
			p->xg = 0.3f;
		}
		else {
			p->xg = 0.0f;
		}

		p->xi += (p->xg + gravX);			// Take Pull On X Axis Into Account
		p->yi += (p->yg + gravY);			// Take Pull On Y Axis Into Account
		p->zi += (p->zg + gravZ);			// Take Pull On Z Axis Into Account
		p->life -= p->fade;		// Reduce Particles Life By 'Fade'

		if (p->life < 0.0f)					// If Particle Is Burned Out
		{
			p->life = 1.0f;					// Give It New Life
			p->fade = float(rand() % 100) / 1000.0f + 0.003f;	// Random Fade Value
			p->x = posX;						// Center On X Axis
			p->y = posY;						// Center On Y Axis
			p->z = posZ;						// Center On Z Axis
			p->xi = xspeed + float((rand() % 60) - 30.0f);	// X Axis Speed And Direction
			p->yi = yspeed + float((rand() % 60) - 30.0f);	// Y Axis Speed And Direction
			p->zi = float((rand() % 60) - 30.0f);	// Z Axis Speed And Direction
			p->r = white[0];
			p->g = white[1];
			p->b = white[2];
			p->position.ToZero();
		}
		else if (p->life < 0.4f)
		{
			p->r = red[0];
			p->g = red[1];
			p->b = red[2];

		}
		else if (p->life < 0.6f)
		{
			p->r = orange[0];
			p->g = orange[1];
			p->b = orange[2];

		}
		else if (p->life < 0.75f)
		{
			p->r = yellow[0];
			p->g = yellow[1];
			p->b = yellow[2];

		}
		else if (p->life < 0.85f)
		{
			p->r = blue[0];
			p->g = blue[1];
			p->b = blue[2];

		}
		++i;
	}
}

Particle* ParticleEmitter::GetNewParticle() {
	Particle* p = NULL;
	p = new Particle();

	p->life = 1.0f;
	p->fade = float(rand() % 100) / 1000.0f + 0.003f;
	p->r = white[0];
	p->g = white[1];
	p->b = white[2];
	p->colour = Vector4(p->r, p->g, p->b, 1.0);
	p->xi = float((rand() % 50) - 25.0f) * 10.0f;
	p->yi = float((rand() % 50) - 25.0f) * 10.0f;
	p->zi = float((rand() % 50) - 25.0f) * 10.0f;
	p->xg = 0.0f;
	p->yg = 0.8f;
	p->zg = 0.0f;
	p->position.ToZero();

	return p;
}

void	ParticleEmitter::ResizeArrays() {
	delete[] vertices;
	delete[] colours;

	glDeleteBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glDeleteBuffers(1, &bufferObject[COLOUR_BUFFER]);

	vertices = new Vector3[particles.size()];
	colours = new Vector4[particles.size()];

	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Vector3), 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Vector4), 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleEmitter::Draw() {
	for (unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i] = particles.at(i)->colour;
	}

	glBindVertexArray(arrayObject);

	//Bind vertex data, update data
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Vector3), (void*)vertices);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);	//Tell the VAO we have positions...
	glEnableVertexAttribArray(VERTEX_BUFFER);	//Binds this buffer to the VAO
	glObjectLabel(GL_BUFFER, bufferObject[VERTEX_BUFFER], -1, "Positions");

	//update colours
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Vector4), (void*)colours);
	glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
	glEnableVertexAttribArray(COLOUR_BUFFER);
	glObjectLabel(GL_BUFFER, bufferObject[COLOUR_BUFFER], -1, "Colours");

	//blend func
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//Draw 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_POINTS, 0, particles.size());
	glBindTexture(GL_TEXTURE_2D, 0);

	// turn off VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
};