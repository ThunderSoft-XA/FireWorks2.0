/*
 * particle.h
 *
 *  Created on: Dec 9, 2019
 *      Author: little
 */

#ifndef LIB_INCLUDE_PARTICLE_H_
#define LIB_INCLUDE_PARTICLE_H_

#include <assert.h>




#include "gl_aux.h"




namespace little
{



using namespace gl_aux;



class ParticleEngine;



class Particle
{
public:
	Particle() :
		life(0.0f),
		active(false),
		angle(0)
	{
		//id = count++;

		//FOOTPRINT();
		//this->Dump();
	}

	virtual ~Particle()
	{

	}

public:
	void Initialize(const ParticleEngine & engine);
	void Update(const ParticleEngine & engine, float dt);

	float Life(void)     const    { return this->life; }
	bool  IsActive(void) const    { return this->active; }

	const Vertex  & GetVertex()   const { return this->vertex;   }
	const Vector3 & GetVelocity() const { return this->velocity; }

	void  Dump() const
	{
#ifndef NDEBUG
		PRINT_SEPARATOR();
		LOG_INFO("ID:", id);
		this->vertex.Dump();
		this->velocity.Dump(false);
		std::cout << "Life:" << this->life << ", Active:" << this->active << std::endl;
		PRINT_SEPARATOR();
#endif  //NDEBUG
	}

protected:
	virtual void Colorize(const ParticleEngine & engine);
	virtual void InitVelocity(const ParticleEngine & engine);
	virtual void Position(const ParticleEngine & engine, float dt);

public:
	static unsigned int NumberOfVertices() { return sizeof(Vertices) / sizeof(Vertex); }

protected:
	int     id;
	Vertex  vertex;
	Vector3 velocity;

	float   life;
	bool    active;

	float   angle;

	//static int count;

public:
	class Vertices
	{
	private:
		Vertices()
		{

		}

	public:
		Vertex v0, v1, v2, v3;
	};
};



class AnisotropicParticle : public Particle
{
protected:
	virtual void Colorize(const ParticleEngine & engine);
};



class ParticleEngine : public Drawable
{
public:
	ParticleEngine(unsigned int num_particles, Particle * particles) :
		num_particles(num_particles),
		num_active_particles(0),
		particles(particles),
		particle_size(0.1),//6.0f),
		initial_height(0.0f),//16.0f),
		life_span(8.0f),
		min_age(0.f),
		friction(0.6f),
		texture_id(0)
	{
		FOOTPRINT();

		assert(this->particles);
		assert(this->num_particles >= 1);

		unsigned int num_vertices = this->num_particles * Particle::NumberOfVertices();

		this->vertices = new Vertex[num_vertices];

		assert(this->vertices);
		LOG_INFO("NumOfVertices:", num_vertices);
	}

	~ParticleEngine()
	{
		FOOTPRINT();

		SAFE_DEL_ARRAY(this->vertices);
	}

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs, GLsizei tex_width, GLsizei tex_height, const unsigned char * texture);
	virtual void Draw();

public:
	void Update(double t, float dt, mat4x4 mv);

	unsigned int NumOfParticles(void) const { return this->num_particles; }

	float InitialHeight(void) const    { return this->initial_height; }
	float LifeSpan(void)      const    { return this->life_span;      }
	float MinAge(void)        const    { return this->min_age;        }
	float Friction(void)      const    { return this->friction;       }
	float ParticleSize(void)  const    { return this->particle_size;  }

	void SetInitialHeight(float v)     { this->initial_height = v; }
	void SetLifeSpan(float v)          { this->life_span      = v; }
	void SetMinAge(float v)            { this->min_age        = v; }
	void SetFriction(float v)          { this->friction       = v; }
	void SetParticleSize(float v)      { this->particle_size  = v; }

	operator Vertex*() const           { return this->vertices; }

	void DumpVertices() const;

protected:
	float BirthInterval(void) const    { return this->life_span/this->num_particles; }
	float MaxDeltaTime(void)  const    { return this->BirthInterval() * 0.5f;         }

protected:
	unsigned int  num_particles;
	unsigned int  num_active_particles;
	Particle     *particles;
	Vertex       *vertices;

	float initial_height;
	float life_span;
	float min_age;
	float friction;
	float particle_size;

	GLuint texture_id;
};






};


#endif /* LIB_INCLUDE_PARTICLE_H_ */
