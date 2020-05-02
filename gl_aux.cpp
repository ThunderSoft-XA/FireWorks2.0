/*
 * gl_aux.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */

#include "gl_aux.h"







namespace gl_aux
{



uint64_t GetRawTime() //Microseconds
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (uint64_t) tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec;
}



double GetTime() //Seconds
{
	static uint64_t	base_time = 0;

	if ( 0 == base_time )
	{
		base_time = GetRawTime();
	}

	return (double)(GetRawTime() - base_time) * 1e-6; // Microsecond
}



GLuint Program::AttachShader(GLuint program, GLenum type, const GLchar * src)
{
	GL_CHECK_CONDITION(src != 0, "Shader source is missing!");

    auto attach_shader = [](GLuint program, GLenum shader_type, const GLchar **shader_source)
	{
		GLuint shader = glCreateShader(shader_type);

		glShaderSource(shader, 1, shader_source, 0);
		glCompileShader(shader);
		GL_CHECK_SHADER_CONDITION(shader, GL_COMPILE_STATUS, GL_FALSE);
		glAttachShader(program, shader);

		return shader;
	};

    return attach_shader(program, type, &src);
}

void Program::DetachShader(GLuint program, GLuint & shader)
{
	if (program && shader)
	{
		glDetachShader(program, shader);
		glDeleteShader(shader);
		shader = 0;
	}
}

bool Program::Link(const GLchar * vs_src, const GLchar * fs_src)
{
	bool status;

	if (!this->id)
	{
		status = this->Init();

		if (!status) return false;
	}

	GL_CHECK_CONDITION(this->id != 0, "Program doesn't initialized!");

	if (vs_src)
		this->vs_id = Program::AttachShader(this->id, GL_VERTEX_SHADER, vs_src);
	if (fs_src)
		this->fs_id = Program::AttachShader(this->id, GL_FRAGMENT_SHADER, fs_src);

    glLinkProgram(this->id);

    GL_CHECK_PROGRAM_CONDITION(this->id, GL_LINK_STATUS, GL_FALSE);

    return true;
}



void Xform::LookAt(vec3 eye, vec3 center, vec3 up)
{
	if (!eye)
	{
		static vec3 _eye = {1.0f, 1.0f, 1.0f};
		eye = _eye;
	};
	if (!center)
	{
		static vec3 _center = {0.0f, 0.0f, 0.0f};
		center = _center;
	};
	if (!up)
	{
		static vec3 _up = {0.0f, 0.0f, 1.0f};
		up = _up;
	};

	mat4x4_look_at(this->view, eye, center, up);
}

void Xform::Update(std::function<void (size_t)> callback)
{
	if (callback && this->selector % NUM_ANGLE == 0 && 1 == this->toggle)
	{
		this->cycle++;

		callback(this->cycle);
	}

	auto inc_selector = [&]()
		{
			this->selector = (this->selector + 1) % NUM_ANGLE;
		};

	// Disable rotation
	if (!status[this->selector])
	{
		inc_selector();
		this->toggle = 1;

		//LOG_INFO("Selector:", this->selector);

		return;
	}

	Twins<GLfloat> & angle = *angles[selector];

	if (1 == this->toggle)
	{
		angle.Reset();
		this->adjusted_threhold = angle + coefs[selector] * this->threshold;
		this->toggle = 0;

		LOG_INFO("Selector:", this->selector);
		LOG_INFO("Threshold:", this->threshold);
	}

	angle += coefs[selector] * this->Delta;

	if (std::abs(angle) >= std::abs(this->adjusted_threhold))
	{
		inc_selector();
		this->toggle = 1;

		LOG_INFO("Selector:", this->selector);
	}
}

void Xform::UpdateModel()
{
	mat4x4_identity(this->model);
	mat4x4_translate(this->model, 0.0f, 0.0f, this->Zoom);
	mat4x4_rotate_X(this->model, this->model, radian(this->alpha));
	mat4x4_rotate_Y(this->model, this->model, radian(this->beta));
	mat4x4_rotate_Z(this->model, this->model, radian(this->gamma));
}



bool Drawable::Init(const GLchar * vs, const GLchar * fs)
{
	bool status;

    status = this->program.Init();
    if (!status) return false;

	status = this->program.Link(vs, fs);
	if (!status) return false;

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	return this->VAO && this->VBO;
}

GLint Drawable::GetUniformLocation(const GLchar * name)
{
	GLuint id = this->program.ID();

	this->program.Use();
	this->MVP = glGetUniformLocation(id, name);
	PROGRAM_LOG(id, "GetUniformLocation");

	LOG_INFO("Program:", id);
	LOG_INFO("UniformName:", name);
	LOG_INFO("Location:", this->MVP);

	return this->MVP;
}

void Drawable::MvpUniformMatrix(const GLfloat * mvp)
{
	this->program.Use();
	glUniformMatrix4fv(this->MVP, 1, GL_FALSE, (const GLfloat *)mvp);
}



void Renderer::Initialize(unsigned int view_width, unsigned int view_height)
{
	this->view_width  = view_width;
	this->view_height = view_height;

	GL_DUMP(this->view_width, this->view_height);

	struct timeval tv;

	gettimeofday(&tv, NULL);
	srand(tv.tv_sec);

	//glEnable(GL_DEBUG_OUTPUT);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glViewport(0, 0, this->view_width, this->view_height);

	// The default front face is GL_CCW
	//glFrontFace(GL_CW); //GL_CCW); //

	this->OnInit();

	GL_CHECK_ERRORS();
}



void Renderer::Draw()
{
	double t  = GetTime();
	float  dt = t - last_time;

	this->last_time = t;

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glViewport(0, 0, this->view_width, this->view_height);

    this->OnDraw(t, dt);

    GL_CHECK_ERRORS_ONESHOT();
}



Vector2 LL_TEX_COORD(0.0f, 0.0f);
Vector2 LR_TEX_COORD(1.0f, 0.0f);
Vector2 UL_TEX_COORD(0.0f, 1.0f);
Vector2 UR_TEX_COORD(1.0f, 1.0f);



} //namespace gl_aux

