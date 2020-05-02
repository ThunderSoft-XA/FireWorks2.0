/*
 * particle.cpp
 *
 *  Created on: Dec 9, 2019
 *      Author: little
 */

#include "particle.h"
#include "math_aux.h"

#include "physics.h"




using namespace little;



#ifndef NDEBUG
int Particle::count = 0;
#endif  //NDEBUG



void Particle::Initialize(const ParticleEngine & engine)
{
	this->vertex.position.x = 0;
	this->vertex.position.y = 0;
	this->vertex.position.z = engine.InitialHeight();

	this->angle = 2 * M_PI * Random();

	this->Colorize(engine);
	this->InitVelocity(engine);

	this->life   = 1.0f;
	this->active = true;
}

void Particle::Update(const ParticleEngine & engine, float dt)
{
	// Ignore inactive particles
	if (!this->IsActive()) return;

	// Particle is growing up
	this->life -= dt * (1.f / engine.LifeSpan());

	if (this->life <= 0.f)
	{
		this->active = false;
		return;
	}

	this->Position(engine, dt);
}

void Particle::Colorize(const ParticleEngine & engine)
{
	double t = GetTime();

	this->vertex.color.r = Sine(0.22, 0.1, t);
	this->vertex.color.g = Sine(0.67, 1.1, t);
	this->vertex.color.b = Sine(0.95, 1.9, t);
}


void Particle::InitVelocity(const ParticleEngine & engine)
{
	double t      = GetTime();
	double period = 2 * M_PI / engine.LifeSpan();
	float  scale  = (0.9f + 0.1f * (float) (sin(period * t) + sin(period / 2.6f * t)));

	this->velocity.x = 0.3 * (float)cos(this->angle);
	this->velocity.y = 0.3 * (float)sin(this->angle);
	this->velocity.z = 1.6f + 0.1f * Random();

	this->velocity *= scale;
}


void Particle::Position(const ParticleEngine & engine, float dt)
{
	// Apply the gravity
	// vv = vv - g * t
	this->velocity.z -= 0.1 * VerticalVelocity(dt);

	// Update position
	this->vertex.position += this->velocity * dt;

//	this->velocity.Dump();
//	this->vertex.position.Dump();

	// Bounce
	if (this->velocity.z < 0.f)
	{
		float phs      = engine.ParticleSize() / 2.0f;
		float friction = engine.Friction();

		if (this->vertex.position.z <= phs)
		{
			this->velocity.z        *= -friction;
			this->vertex.position.z  = phs + friction * (phs - this->vertex.position.z);
		}
	}
}



void AnisotropicParticle::Colorize(const ParticleEngine & engine)
{
	double t = GetTime();

	this->vertex.color.r = Sine(0.22, t, this->angle);
	this->vertex.color.g = Sine(0.67, t + 1.1, this->angle);
	this->vertex.color.b = Sine(0.95, t + 1.9, this->angle);
}



bool ParticleEngine::Init(const GLchar * vs, const GLchar* fs, GLsizei tex_width, GLsizei tex_height, const unsigned char * texture)
{
	bool status;

	status = Drawable::Init(vs, fs);
	if (!status) return false;

	glGenTextures(1, &this->texture_id);

	glBindTexture(GL_TEXTURE_2D, this->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//GL_REPEAT);//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//GL_REPEAT);//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST);//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST);//
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_width, tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texture);

    return this->texture_id;
}


void ParticleEngine::Draw()
{
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	Vertex::VertexAttribPointers();

	GLsizeiptr num_vertices = this->num_active_particles * Particle::NumberOfVertices();
	GLsizeiptr data_size    = num_vertices * sizeof(Vertex);

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);//GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, this->vertices);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture_id);

    unsigned int num_vertices_per_particle = Particle::NumberOfVertices();

    for (int i = 0; i < this->num_active_particles; i++)
    {
    	glDrawArrays(GL_TRIANGLE_FAN, i * num_vertices_per_particle, num_vertices_per_particle);
    }

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}


/**
 * (0,1)   (1,1)
 * 3-------2
 * |      /|
 * |2nd  / |
 * |    /  |
 * |   /   |
 * |  /    |
 * | / 1st |
 * |/      |
 * 0-------1
 * (0,0)   (1,0)
 */
void ParticleEngine::Update(double t, float dt, mat4x4 mv)
{
	float max_delta_time = this->MaxDeltaTime();
	float birth_interval = this->BirthInterval();

	for (float delta; dt > 0.f; dt -= delta)
	{
		delta = dt > max_delta_time ? max_delta_time : dt;

		for (int i = 0; i < this->NumOfParticles(); i++)
		{
			this->particles[i].Update(*this, delta);
		}

		this->min_age += delta;

		while (this->min_age >= birth_interval)
		{
			this->min_age -= birth_interval;

			for (int i = 0; i < this->NumOfParticles(); i++)
			{
				Particle & p = this->particles[i];

				if (!p.IsActive())
				{
					p.Initialize(*this);
					p.Update(*this, this->MinAge());

					break;
				}
			}
		}
	}

	ParallelView parallel(this->particle_size, this->particle_size, mv);

	Particle           *p  = this->particles;
	Particle::Vertices *pv = (Particle::Vertices*)this->vertices;
	// Reset the number of active particles
	this->num_active_particles = 0;
	for (int i = 0; i < this->NumOfParticles(); i++, p++)
	{
		if (!p->IsActive()) continue;

#if 1
		const Vertex & vertex = p->GetVertex();
#else
		Vertex vertex = p->GetVertex();
		vertex.position = {0, 0, 0};
#endif

		// 0: Lower left
		pv->v0.position = vertex.position + parallel.LowerLeft();
		pv->v0.color    = vertex.color;
		pv->v0.texture  = LL_TEX_COORD;

		// 1: Lower right
		pv->v1.position = vertex.position + parallel.LowerRight();
		pv->v1.color    = vertex.color;
		pv->v1.texture  = LR_TEX_COORD;

		// 2: Upper right
		pv->v2.position = vertex.position + parallel.UpperRight();
		pv->v2.color    = vertex.color;
		pv->v2.texture  = UR_TEX_COORD;

		// 3: Upper left
		pv->v3.position = vertex.position + parallel.UpperLeft();
		pv->v3.color    = vertex.color;
		pv->v3.texture  = UL_TEX_COORD;

		this->num_active_particles++;
		pv++;
	}

	//this->DumpVertices();
}


void ParticleEngine::DumpVertices() const
{
#ifndef NDEBUG
	unsigned int count = this->num_active_particles * Particle::NumberOfVertices();
	if (count == 0) return;
	PRINT_SEPARATOR();
	for (int i = 0; i < count; i++)
	{
		LOG_INFO("Index:", i);
		this->vertices[i].Dump();
	}
	PRINT_SEPARATOR();
#endif  //NDEBUG
}

