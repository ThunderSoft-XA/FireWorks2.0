/*
 * mesh.h
 *
 *  Created on: Oct 31, 2019
 *      Author: little
 */

#ifndef INCLUDE_MESH_H_
#define INCLUDE_MESH_H_

#include "gl_aux.h"




using namespace gl_aux;


namespace little
{



#ifdef GL_ES
typedef GLushort INDEX_T;
#else  //!GL_ES
typedef GLuint   INDEX_T
#endif //GL_ES



class Mesh : public Drawable
{
public:
	Mesh(unsigned int width, unsigned int height) :
		width(width),
		height(height),
		num_vertices(this->width * this->height),
		indexes(0),
		scale(1.0f),
		mode(0)
	{
		FOOTPRINT();

		this->vertices = new Vertex[this->width * this->height]();
	}

	virtual ~Mesh()
	{
		FOOTPRINT();

		SAFE_DEL_ARRAY(vertices);
		SAFE_DEL_ARRAY(indexes);
	}

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs);
	virtual void Draw();

	unsigned int GetWidth()      const        { return this->width; }
	unsigned int GetHeight()     const        { return this->height; }
	unsigned int NumOfVertices() const        { return this->num_vertices; }
	unsigned int VertexPosition(int x, int y) { return y * this->width + x; };

	float  GetScale(void)    const { return this->scale; }
	void   SetScale(float s)       { this->scale = s;    }

	GLenum GetMode(void)     const { return this->mode;  }
	void   SetMode(GLenum m)       { this->mode = m;     }

	GLsizeiptr Stride()                     const { return 6 * sizeof(GLfloat); }
	GLint      NumOfComponentsPerPosition() const { return 3; }
	GLint      NumOfComponentsPerColor()    const { return 3; }

	Vertex *        GetVertices()       { return this->vertices; }
	const INDEX_T * GetIndexes()  const { return this->indexes; }

	virtual unsigned int NumOfIndexes() const { return 2 * (this->width * (this->height - 1)) + 2 * (this->height - 2); }

	virtual void Dump() const;
	virtual void DumpIndexes() const;

	void DumpVertex(int pos) const;

public: // Operator
	operator GLsizeiptr() { return this->NumOfVertices() * sizeof(Vertex); }

protected:
	virtual void UpdateZvalue(int x, int y, int pos)	{ this->vertices[pos].position.z = 0.f; }
	virtual void ColorizeVertex(int x, int y, int pos, bool gradient = true);
	virtual void ComputeIndex(int x, int y); // For Triangle Strip

protected:
	unsigned int width;
	unsigned int height;

	unsigned int num_vertices;

	Vertex	* vertices;
	INDEX_T	* indexes;

	float  scale;
	GLenum mode;
};



class AnimatedMesh : public Mesh
{
public:
	AnimatedMesh(unsigned int width, unsigned int height) :
		Mesh(width, height),
		PressureThreshold(width * 0.1),
		PressurePeriod(width / 50),
		PressureCoefficient(0.01),
		MaxDeltaTime(0.05),
		TimeStepMultiple(10),
		pressure(width, height),
		velocity(width, height),
		last_time(0),
		delta_time(0),
		reset_data(false),
		frame_count(0),
		max_frame_num(-1)
	{
		FOOTPRINT();

		this->last_time = GetTime();
	}

public:
	virtual bool Init(const GLchar * vs, const GLchar* fs);
	virtual void Update(void);

public:
	void EnableResetData(bool enable) { this->reset_data = enable; }

	unsigned int GetMaxFrameNumber(void)             const { return this->max_frame_num; }
	void         SetMaxFrameNumber(unsigned int num)       { this->max_frame_num = num; }

public:
	struct DualMatrix
	{
		DualMatrix(unsigned int width, unsigned int height) :
			x(width, height),
			y(width, height)
		{
		}

		Matrix<double> x;
		Matrix<double> y;
	};

protected:
	virtual void InitPressure(void);
	virtual void ComputePressure(void);
	virtual void ApplyPressure(void);

public:
	double PressureThreshold;
	double PressurePeriod;
	double PressureCoefficient;
	double MaxDeltaTime;
	double TimeStepMultiple;

protected:
	Matrix<double> pressure;

	DualMatrix velocity;

	double last_time;
	double delta_time;

	bool reset_data;

	unsigned int frame_count;
	unsigned int max_frame_num;
};



class FilteringMesh: public AnimatedMesh
{
public:
	FilteringMesh(unsigned int width, unsigned int height) :
		AnimatedMesh(width, height)
	{
		FOOTPRINT();
	}

protected:
	virtual void UpdateZvalue(int x, int y, int pos);

	virtual void ApplyPressure(void);
};




} // namespace little




#endif /* INCLUDE_MESH_H_ */
