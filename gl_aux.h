/*
 * gl_aux.h
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */

#ifndef INCLUDE_GL_AUX_H_
#define INCLUDE_GL_AUX_H_






#include <math.h>
#include <iostream>

#include <stdlib.h>
#include <sstream> 
#include <fstream>
#ifdef _OGLES3
#include "OpenGLES/FrmGLES3.h"
//#include <GLES3/gl3.h>
//#include <GLES2/gl2ext.h>
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#endif

#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
//#include "glm/detail/func_geometric.hpp"
#include "glm/geometric.hpp"

#include <cmath>
#include <functional>
#include <sys/time.h>
#include "linmath.h"


#ifndef NDEBUG

#define __DEBUG_PRINT(stream, tag, m, v) \
	do { \
		stream << "[" << tag << "][" << __FILE__ << "@" << __LINE__ << "] " << (m) << " " << (v) << std::endl; \
	} while(0)

#define LOG_INFO(m, v)	__DEBUG_PRINT(std::cout, "INFO", m, v)
#define LOG_ERROR(m, v)	__DEBUG_PRINT(std::cerr, "ERROR", m, v)

#define PRINT_SEPARATOR() LOG_INFO("----------------------------------------------------", "")

#else //!NDEBUG

#define LOG_INFO(m, v)
#define LOG_ERROR(m, v)

#define PRINT_SEPARATOR()

#endif //NDEBUG



#ifndef NDEBUG

#define __GL_LOG(get_iv, get_log, obj, m) \
	do { \
		GLsizei len; \
		get_iv(obj, GL_INFO_LOG_LENGTH, &len); \
		/*LOG_INFO("Length:", len);*/ \
		if (len > 0) \
		{ \
			GLchar *log = new GLchar[len + 1]; \
			get_log(obj, len, &len, log); \
			/*std::cerr << "[ERROR][" << __FILE__ << "@" << __LINE__ << "] " << m << ": " << log << std::endl; */ \
			LOG_ERROR(m, log); \
			delete [] log; \
		} \
	} while(0)

#define SHADER_LOG(_shader, m)		__GL_LOG(glGetShaderiv, glGetShaderInfoLog, _shader, m)

#define PROGRAM_LOG(_program, m)	__GL_LOG(glGetProgramiv, glGetProgramInfoLog, _program, m)

#define GL_CHECK_ERRORS() \
	do { \
		GLenum e; \
		unsigned int finite = 255; \
		while((e = glGetError()) != GL_NO_ERROR && finite--) \
		{ \
			LOG_ERROR("[GL ERROR]", e); \
		} \
	} while(0)

#define GL_CHECK_ERRORS_ONESHOT() \
	do { \
		static int toggle = 1; \
		if (toggle) \
		{ \
			toggle = 0; \
			GL_CHECK_ERRORS(); \
		} \
	} while(0)

#define GL_CHECK_SHADER_CONDITION(_shader, pname, error_condition) \
	do { \
		GLint params; \
		glGetShaderiv(_shader, pname, &params); \
		if (params == error_condition) SHADER_LOG(_shader, ""); \
	} while(0)

#define GL_CHECK_PROGRAM_CONDITION(_program, pname, error_condition) \
	do { \
		GLint params; \
		glGetProgramiv(_program, pname, &params); \
		if (params == error_condition) PROGRAM_LOG(_program, ""); \
	} while(0)

#else //!NDEBUG

#define SHADER_LOG(_shader, m)
#define PROGRAM_LOG(_program, m)
#define GL_CHECK_ERRORS()
#define GL_CHECK_ERRORS_ONESHOT()
#define GL_CHECK_SHADER_CONDITION(_shader, pname, error_condition)
#define GL_CHECK_PROGRAM_CONDITION(_program, pname, error_condition)

#endif //_DEBUG


#define GL_CHECK_CONDITION(condition, msg) \
	do { \
		if (!(condition)) { \
			LOG_ERROR("[GL ERROR]", msg); \
		} \
	} while(0)


#ifdef NDEBUG
#define FOOTPRINT()
#else //!NDEBUG
#define FOOTPRINT()	std::cout << "[INFO][" << __FILE__ << "@" << __LINE__ << "][" << __func__ << "]" << std::endl;
#endif //NDEBUG


#ifndef SAFE_DEL_ARRAY
#define SAFE_DEL_ARRAY(a) if (0 != (a)) { delete[] (a); (a) = 0; }
#endif //SAFE_DEL_ARRAY


#ifndef GL_ES
#define GL_ES
#endif


#ifdef GL_ES
#if 1
#define STRINGIZE(shader)	"#version 310 es\n" "#pragma debug(on)\n" "precision mediump float;\n" #shader
#else
#define STRINGIZE(shader)	"#version 310 es\n" #shader
#endif
#else
#define STRINGIZE(shader)	"#version 330 core\n" #shader
#endif


#ifdef GL_ES
typedef GLushort INDEX_T;
#else  //!GL_ES
typedef GLuint   INDEX_T
#endif //GL_ES



namespace gl_aux
{



typedef struct _Vector3
{
	_Vector3() :
		x(0.0f),
		y(0.0f),
		z(0.0f)
	{

	}

	_Vector3(GLfloat x, GLfloat y, GLfloat z):
		x(x),
		y(y),
		z(z)
	{

	}

	union {	GLfloat x; GLfloat r; };
	union {	GLfloat y; GLfloat g; };
	union {	GLfloat z; GLfloat b; };

	_Vector3 & operator= (const _Vector3 & other)
	{
		if (this != &other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
		}

		return *this;
	}

	_Vector3 & operator+= (const _Vector3 & other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;

		return *this;
	}

	_Vector3 & operator-= (const _Vector3 & other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;

		return *this;
	}

	_Vector3 & operator*= (float other)
	{
		this->x *= other;
		this->y *= other;
		this->z *= other;

		return *this;
	}

	friend _Vector3 operator+ (_Vector3 left, const _Vector3 & right)
	{
		left += right;

		return left;
	}

	friend _Vector3 operator- (_Vector3 left, const _Vector3 & right)
	{
		left -= right;

		return left;
	}

	friend _Vector3 operator* (_Vector3 left, float right)
	{
		left *= right;

		return left;
	}

	void Dump(bool color = false) const
	{
#ifndef NDEBUG
		if (color)
		{
			std::cout << "R:" << this->r << ", "
					  << "G:" << this->g << ", "
					  << "B:" << this->b << std::endl;
		}
		else
		{
			std::cout << "X:" << this->x << ", "
					  << "Y:" << this->y << ", "
					  << "Z:" << this->z << std::endl;
		}
#endif  //NDEBUG
	}
} Vector3;


typedef struct _Vector2
{
	_Vector2() :
		x(0.0f),
		y(0.0f)
	{

	}

	_Vector2(GLfloat x, GLfloat y):
		x(x),
		y(y)
	{

	}

	union {	GLfloat x; GLfloat s; GLfloat w; };
	union {	GLfloat y; GLfloat t; GLfloat h; };

	_Vector2 & operator= (const _Vector2 & other)
	{
		if (this != &other)
		{
			this->x = other.x;
			this->y = other.y;
		}

		return *this;
	}

	_Vector2 & operator+= (const _Vector2 & other)
	{
		this->x += other.x;
		this->y += other.y;

		return *this;
	}

	_Vector2 & operator-= (const _Vector2 & other)
	{
		this->x -= other.x;
		this->y -= other.y;

		return *this;
	}

	_Vector2 & operator*= (float other)
	{
		this->x *= other;
		this->y *= other;

		return *this;
	}

	friend _Vector2 operator+ (_Vector2 left, const _Vector2 & right)
	{
		left += right;

		return left;
	}

	friend _Vector2 operator- (_Vector2 left, const _Vector2 & right)
	{
		left -= right;

		return left;
	}

	friend _Vector2 operator* (_Vector2 left, float right)
	{
		left *= right;

		return left;
	}

	void Dump(bool st = false) const
	{
#ifndef NDEBUG
		if (st)
		{
			std::cout << "S:" << this->s << ", "
					  << "T:" << this->t << std::endl;
		}
		else
		{
			std::cout << "X:" << this->x << ", "
					  << "Y:" << this->y << std::endl;
		}
#endif  //NDEBUG
	}
} Vector2;


typedef struct _Vertex
{
	Vector3 position;
	Vector3 color;
	Vector2 texture;

	_Vertex & operator= (const _Vertex & other)
	{
		if (this != &other)
		{
			this->position = other.position;
			this->color    = other.color;
			this->texture  = other.texture;
		}

		return *this;
	}

	void Dump() const
	{
#ifndef NDEBUG
		LOG_INFO("Position:", "");
		this->position.Dump(false);
		LOG_INFO("Color:", "");
		this->color.Dump(true);
		LOG_INFO("Texture:", "");
		this->texture.Dump(true);
#endif  //NDEBUG
	}

	static void VertexAttribPointers()
	{
		GLsizeiptr stride = sizeof(_Vertex);

		// Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		// Color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)sizeof(Vector3));
		// Texture coordinate
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(Vector3) + sizeof(Vector3)));
	}
} Vertex;


template <class T>
class Matrix
{
public:
	Matrix(unsigned int num_row, unsigned int num_col)	:
		num_of_row(num_row),
		num_of_col(num_col),
		data(0)
	{
		FOOTPRINT();

		if(this->num_of_row > 0 && this->num_of_col > 0)
			data = new T[this->num_of_row * this->num_of_col]();
	}

	~Matrix()
	{
		FOOTPRINT();
		SAFE_DEL_ARRAY(data);
	}

public:
	// Indexing (parenthesis operator)
	const T& operator () (unsigned int nr, unsigned int nc) const
	{
		return data[ nr * num_of_col + nc ];
	}

	T& operator () (unsigned int nr, unsigned int nc)
	{
		return data[ nr * num_of_col + nc ];
	}

public:
	// Get dimensions
	unsigned int NumOfRow() const { return num_of_row; }
	unsigned int NumOfCol() const { return num_of_col; }

	void Reset(void)
	{
		for (unsigned int i = 0; i < this->num_of_row * this->num_of_col; i++)
			this->data[i] = 0;
	}

private:
	unsigned int num_of_row;
	unsigned int num_of_col;

	T* data;

private:
	// To prevent unwanted copying:
	Matrix(const Matrix<T> & m);
	Matrix& operator = (const Matrix<T> & m);
};



template <class T>
class Twins
{
public:
	Twins() :
		value(0),
		original(0)
	{

	}

public:
	T operator = (T v)
	{
		this->value = this->original = v;

		//INFO("Value:" << this->value << ", Original:" << this->original);

		return this->value;
	}

	operator T()
	{
		return this->value;
	}

	T operator += (T v)
	{
		this->value += v;

		return this->value;
	}

	T operator -= (T v)
	{
		this->value -= v;

		return this->value;
	}

public:
	void Reset(void)
	{
		this->value = this->original;

		//INFO("Value:" << this->value << ", Original:" << this->original);
		//INFO("");
	}

protected:
	T value;
	T original;
};



auto radian = [](double degree)
{
	return degree * M_PI / 180.0;
};



uint64_t GetRawTime(); //Seconds
double GetTime();      //Microseconds



class ParallelView
{
public:
	ParallelView(float width, float height, mat4x4 model_view)
	{
		float hw = width / 2.0f;
		float hh = height / 2.0f;

		vec4   ll_vec_o = {-hw, -hh, 0, 0};
		vec4   lr_vec_o = { hw, -hh, 0, 0};
		vec4   ll_vec, lr_vec;
		mat4x4 inv_mv;

		mat4x4_invert(inv_mv, model_view);
		mat4x4_mul_vec4(ll_vec, inv_mv, ll_vec_o);
		mat4x4_mul_vec4(lr_vec, inv_mv, lr_vec_o);

		this->lower_left  = Vector3(ll_vec[0], ll_vec[1], ll_vec[2]);
		this->lower_right = Vector3(lr_vec[0], lr_vec[1], lr_vec[2]);
		this->upper_left  = this->lower_right * -1;
		this->upper_right = this->lower_left  * -1;
	}

public:
	const Vector3 & LowerLeft(void)  const { return this->lower_left;  }
	const Vector3 & LowerRight(void) const { return this->lower_right; }
	const Vector3 & UpperLeft(void)  const { return this->upper_left;  }
	const Vector3 & UpperRight(void) const { return this->upper_right; }

protected:
	Vector3 lower_left;
	Vector3 lower_right;
	Vector3 upper_left;
	Vector3 upper_right;
};



class Program
{
public:
	Program() :
		id(0),
		vs_id(0),
		fs_id(0)
	{

	}

	~Program()
	{
		Program::DetachShader(this->id, this->vs_id);
		Program::DetachShader(this->id, this->fs_id);

		if (this->id)
		{
			glDeleteProgram(this->id);
			this->id = 0;
		}
	}

public:
	bool   Link(const GLchar * vs_src, const GLchar * fs_src);

	bool   Init(void)      { return this->id = glCreateProgram(); }
	void   Use(void) const { glUseProgram(this->id);       }
	GLuint ID(void)  const { return this->id;              }

public:
	static GLuint AttachShader(GLuint program, GLenum type, const GLchar * src);
	static void   DetachShader(GLuint program, GLuint & shader);

protected:
	GLuint id;
	GLuint vs_id;
	GLuint fs_id;
};



struct Xform
{
	Xform() :
		Zoom(0.0f),
		Delta(1.0f),
		threshold(360),
		adjusted_threhold(0),
		toggle(1),
		selector(0),
		cycle(0),
		angles{&gamma, &beta, &alpha},
		coefs{1, -1, 1},
		status{true, true, true}
	{
		mat4x4_identity(this->model);
		mat4x4_identity(this->view);
		mat4x4_identity(this->projection);
	}

	void GetModel(mat4x4 m) { this->UpdateModel(); mat4x4_dup(m, this->model); }
	void SetModel(mat4x4 m)	{ mat4x4_dup(this->model, m); }

	void GetView(mat4x4 v)  { mat4x4_dup(v, this->view);  }
	void LookAt(vec3 eye, vec3 center = 0, vec3 up = 0);

	void Perspective(float y_fov, float aspect, float n = 0.1f, float f = 100.0f)
	{
		mat4x4_perspective(this->projection, radian(y_fov), aspect, n, f);
	}

	void EnableRotations(bool alpha = true, bool beta = true, bool gamma = true)
	{
		status[2] = alpha;
		status[1] = beta;
		status[0] = gamma;
	}

	// Gets ModelView matrix
	void MV(mat4x4 mv)
	{
		this->UpdateModel();
		mat4x4_mul(mv, this->view, this->model);
	}

	// Gets MVP matrix
	void MVP(mat4x4 mvp)
	{
		mat4x4 mv;

		// MV = View * Model
		this->MV(mv);
		// Projection * MV
		mat4x4_mul(mvp, this->projection, mv);
	}

	void Update(std::function<void (size_t)> callback = 0);

	size_t GetCycle() const { return this->cycle; }

protected:
	void UpdateModel(void);

public:
	GLfloat Zoom;
	GLfloat Delta;

protected:
	mat4x4 model;
	mat4x4 view;
	mat4x4 projection;

	Twins<GLfloat> alpha;
	Twins<GLfloat> beta;
	Twins<GLfloat> gamma;

	GLfloat threshold;
	GLfloat adjusted_threhold;

	int toggle;
	int selector;

	size_t cycle;

	static const size_t NUM_ANGLE = 3;

	Twins<GLfloat> * angles[NUM_ANGLE];
	int	             coefs [NUM_ANGLE];
	bool             status[NUM_ANGLE];
};



class Drawable
{
public:
	Drawable() :
		MVP(-1),
		VAO(0),
		VBO(0)
	{

	}

	virtual ~Drawable()
	{

	}

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs);
	virtual void Draw() = 0;

public:
	GLint GetUniformLocation(const GLchar * name);
	void  MvpUniformLocation(const GLchar * name) { this->MVP = this->GetUniformLocation(name); }
	void  MvpUniformMatrix  (const GLfloat * mvp);

public:
	Program program;

protected:
	GLint  MVP;
	GLuint VAO;
	GLuint VBO;
};



class Renderer
{
public:
	Renderer() :
		view_width(0),
		view_height(0),
		last_time(0)
	{
		FOOTPRINT();

		this->last_time = GetTime();

		//LOG_INFO("Time:", this->last_time);
	}

	virtual ~Renderer()
	{

	}

public:
	void Initialize(unsigned int view_width, unsigned int view_height);
	void Draw();

protected:
	virtual void OnInit()                   { };
	virtual void OnDraw(double t, float dt) { };

public:
	Xform xform;

protected:
	unsigned int view_width;
	unsigned int view_height;

	double last_time;
};



extern Vector2 LL_TEX_COORD;
extern Vector2 LR_TEX_COORD;
extern Vector2 UL_TEX_COORD;
extern Vector2 UR_TEX_COORD;



} //gl_aux



#ifndef NDEBUG

#define GL_DUMP(view_width, view_height) \
	do { \
		int major = 0; \
		int minor = 0; \
		int context_profile = 0; \
		int n = 0; \
		\
		glGetIntegerv(GL_MAJOR_VERSION, &major); \
		glGetIntegerv(GL_MINOR_VERSION, &minor); \
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &context_profile); \
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n); \
		\
		PRINT_SEPARATOR(); \
		LOG_INFO("Width:", view_width); \
		LOG_INFO("Height:", view_height); \
		LOG_INFO("OpenGLMajorVersion:", major); \
		LOG_INFO("OpenGLMinorVersion:", minor); \
		LOG_INFO("OpenGLVersion:", glGetString(GL_VERSION)); \
		LOG_INFO("OpenGLVendor:", glGetString(GL_VENDOR)); \
		LOG_INFO("OpenGLRendererName:", glGetString(GL_RENDERER)); \
		LOG_INFO("OpenGLContextProfile:", context_profile); \
		LOG_INFO("OpenGLShadingLanguageVersion:", glGetString(GL_SHADING_LANGUAGE_VERSION)); \
		LOG_INFO("GL_MAX_VERTEX_ATTRIBS:", GL_MAX_VERTEX_ATTRIBS); \
		LOG_INFO("MaxVertexAttribs:", n); \
		PRINT_SEPARATOR(); \
	} while(0)

#define M4x4_DUMP(m) \
	do { \
		PRINT_SEPARATOR(); \
		LOG_INFO("Matrix:", #m); \
		for (int i = 0; i < 4; i++) \
		{ \
			for (int j = 0; j < 4; j++) \
				std::cout << m[i][j] << " "; \
			std::cout << std::endl; \
		} \
		PRINT_SEPARATOR(); \
	} while(0)

#define V4_DUMP(v) \
	do { \
		PRINT_SEPARATOR(); \
		LOG_INFO("Vector:", #v); \
		for (int i = 0; i < 4; i++) \
		{ \
			std::cout << v[i] << " "; \
		} \
		std::cout << std::endl; \
		PRINT_SEPARATOR(); \
	} while(0)

#else  //!NDEBUG

#define GL_DUMP(view_width, view_height)
#define M4x4_DUMP(m)
#define V4_DUMP(v)

#endif //NDEBUG



#endif /* INCLUDE_GL_AUX_H_ */
