/*
 * image_text.cpp
 *
 *  Created on: Dec 24, 2019
 *      Author: little
 */
#include "firework.h"

#include "linmath.h"





#include "particle.h"
#include "mesh.h"




//{{ Shaders

//{{ Particle Shaders
//{{ Vertex Shaders
const GLchar * vs_firework = STRINGIZE(
	uniform mat4 MVP;

	layout (location = 0) in vec4 Position;
	layout (location = 1) in vec4 Color;
	layout (location = 2) in vec2 UV;

	out vec4 FragColor;
	out vec2 TexCoord;

	void main(void)
	{
		FragColor   = Color;
		TexCoord    = UV;
		gl_Position = MVP * Position;
	}
);
//}} Vertex Shaders
//{{ Fragment Shaders
const GLchar * fs_firework = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		vec4 L = texture(Texture, TexCoord);

		Color = L * FragColor; // Color = vec4(FragColor.r, FragColor.g, FragColor.b, L.r);
	}
);
//}} Fragment Shaders
//}} Particle Shaders

//{{ Floor Shaders
//{{ Vertex Shaders
const GLchar * vs_floor = vs_firework;
//}} Vertex Shaders

//{{ Fragment Shaders
const GLchar * fs_floor = STRINGIZE(
	uniform sampler2D Texture;

	in vec4 FragColor;
	in vec2 TexCoord;

	layout (location = 0) out vec4 Color;

	void main(void)
	{
		Color = FragColor;
	}
);
//}} Fragment Shaders
//}} Floor Shaders

//}} Shaders



#define PARTICLE_TEX_WIDTH  8
#define PARTICLE_TEX_HEIGHT 8

// Particle texture
const unsigned char ParticleTexture[PARTICLE_TEX_WIDTH * PARTICLE_TEX_HEIGHT] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x11, 0x22, 0x22, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x77, 0x77, 0x11, 0x11, 0x00,
    0x00, 0x22, 0x77, 0xFF, 0xEE, 0x77, 0x22, 0x00,
    0x00, 0x22, 0x77, 0xEE, 0xFF, 0x77, 0x22, 0x00,
    0x00, 0x11, 0x11, 0x77, 0x77, 0x11, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x22, 0x22, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



using namespace gl_aux;
using namespace little;



#define FIREWORK_VERSION 2
#if FIREWORK_VERSION == 1
#define NUM_PARTICLES 3000
Particle
#else
#define NUM_PARTICLES 2000
AnisotropicParticle
#endif
Particles[NUM_PARTICLES];

class FireworkRenderer : public Renderer
{
public:
	FireworkRenderer() :
		Engine(NUM_PARTICLES, Particles),
		Floor(160, 160)
	{

	}

protected:
	virtual void OnInit();
	virtual void OnDraw(double t, float dt);

public:
	ParticleEngine Engine;
	Mesh           Floor;
};


void FireworkRenderer::OnInit()
{
	this->Engine.Init(vs_firework, fs_firework, PARTICLE_TEX_WIDTH, PARTICLE_TEX_HEIGHT, ParticleTexture);
	this->Engine.MvpUniformLocation("MVP");

	this->Floor.SetScale(11.0f);
	this->Floor.Init(vs_floor, fs_floor);
	this->Floor.MvpUniformLocation("MVP");

    vec3 eye = { 2.9f, 0.0f, 0.2f};

    this->xform.LookAt(eye);
    this->xform.Perspective(90.0f, (float) view_width / view_height, 0.1f, 100.0f);
    this->xform.EnableRotations(false, true, true);
    this->xform.Zoom  = -0.5f;
    this->xform.Delta =  0.1f; //0.3f; //1.0f; //

	PRINT_SEPARATOR();
	PRINT_SEPARATOR();
}


void FireworkRenderer::OnDraw(double t, float dt)
{
    mat4x4 mv, mvp;

    auto updater = [&](size_t cycle)
	{
//    	FOOTPRINT();
//    	LOG_INFO("Cycle:", cycle);
	};

    this->xform.Update(updater);
    this->xform.MV(mv);
    this->xform.MVP(mvp);

    this->Floor.MvpUniformMatrix((const GLfloat *)mvp);
    this->Floor.Draw();

    this->Engine.MvpUniformMatrix((const GLfloat *)mvp);
    this->Engine.Update(t, dt, mv);
    this->Engine.Draw();
}



FireworkRenderer renderer;



void InitScene(unsigned int view_width, unsigned int view_height)
{
	renderer.Initialize(view_width, view_height);
}

void FireWorkDraw()
{
	renderer.Draw();
}
