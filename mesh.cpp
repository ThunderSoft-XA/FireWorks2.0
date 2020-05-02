/*
 * mesh.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: little
 */
#include "mesh.h"
#include "math_aux.h"







using namespace little;



#define FOR_EACH_VERTEX \
	for (int y = 0; y < this->height; y++) \
		for (int x = 0; x < this->width; x++)

bool Mesh::Init(const GLchar * vs, const GLchar* fs)
{
	bool status;

	status = Drawable::Init(vs, fs);

	SAFE_DEL_ARRAY(this->indexes)
	this->indexes = new INDEX_T[this->NumOfIndexes()]();

	FOR_EACH_VERTEX
	{
		int v_pos = this->VertexPosition(x, y);

		this->vertices[v_pos].position.x = this->scale * ZeroCenteredMapping(x, this->width, true);
		this->vertices[v_pos].position.y = this->scale * ZeroCenteredMapping(y, this->height, true);
		this->UpdateZvalue(x, y, v_pos);
		this->ComputeIndex(x, y);
		this->ColorizeVertex(x, y, v_pos, true);

#if 0
		this->DumpVertex(v_pos);
#endif
	}

	return true;
}



void Mesh::Draw()
{
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	Vertex::VertexAttribPointers();

	GLsizeiptr   data_size = (GLsizeiptr)(*this);
    void       * data      = this->GetVertices();

	glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STREAM_DRAW);//GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	if (GL_POINTS == mode)
	{
		glDrawArrays(mode, 0, this->NumOfVertices());
	}
	else
	{
		glDrawElements(mode, this->NumOfIndexes(), GL_UNSIGNED_SHORT, this->GetIndexes());
	}

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}



void Mesh::Dump() const
{
#ifndef NDEBUG
	PRINT_SEPARATOR();
	LOG_INFO("MeshWidth:", this->GetWidth());
	LOG_INFO("MeshHeight:", this->GetHeight());
	LOG_INFO("NumOfVertices:", this->NumOfVertices());
	LOG_INFO("SizeOfVertices:", (GLsizeiptr)this);
	LOG_INFO("NumberOfIndexes:", this->NumOfIndexes());
	PRINT_SEPARATOR();
#endif //NDEBUG
}



void Mesh::DumpIndexes() const
{
#ifndef NDEBUG
	for (int nc = this->GetWidth() * 2 + 2, i = 0; i < this->NumOfIndexes(); i++)
	{
		std::cout << this->GetIndexes()[i] << " ";
		if ((i % nc) == nc - 1)
			std::cout << std::endl;
	}
	std::cout << std::endl;
#endif //NDEBUG
}



void Mesh::DumpVertex(int pos) const
{
#ifndef NDEBUG
	this->vertices[pos].Dump();
#endif //NDEBUG
}



void Mesh::ColorizeVertex(int x, int y, int pos, bool gradient)
{
	const int M = 2;
	int rx = x % M;
	int ry = y % M;

#if 1
	if (gradient)
	{
		this->vertices[pos].color.r = Normalize(x, 0, this->width, true);
	}
	else
	{
		if ((1 == rx || 2 == rx) && (0 == ry || 3 == ry))
		{
			this->vertices[pos].color.r = 0.0f;
		}
		else
		{
			this->vertices[pos].color.r = 1.0f;
		}
	}
	this->vertices[pos].color.g = Normalize(y, 0, this->height, true);
	this->vertices[pos].color.b = 1 - (Normalize(x, 0, this->width, true) + Normalize(y, 0, this->height, true)) / 2.f;
#else
	this->vertices[pos].color.r = 0.1f;
	this->vertices[pos].color.g = 0.5f;
	this->vertices[pos].color.b = 0.5f;
#endif
}



void Mesh::ComputeIndex(int x, int y)
{
	// Populates indexes
	if (y < this->height - 1)
	{
		int i_pos = 2 * this->VertexPosition(x, y);

		if (y > 0) i_pos += (y * 2);

		/**
		 * Next
		 * |
		 * +-------<-------+
		 *                 |
		 * 3-------4-------5 Degenerate/
		 * |      /|      /|
		 * | 1st / | 3rd / |
		 * |    /  |    /  |
		 * |   /   |   /   |
		 * |  /    |  /    |
		 * | / 2nd | / 4th |
		 * |/      |/      |
		 * 0-------1-------2
		 */
		// First triangle
		this->indexes[i_pos + 0] = (y)		* this->width + x;		// Current point
		this->indexes[i_pos + 1] = (y + 1)	* this->width + x;		// Upper

		//LOG_INFO(i_pos + 0, this->indexes[i_pos + 0]);
		//LOG_INFO(i_pos + 1, this->indexes[i_pos + 1]);

		if (this->width - 1 == x && y < this->height - 2)
		{
			// Regenerate triangles
			this->indexes[i_pos + 2] = this->indexes[i_pos + 1];
			this->indexes[i_pos + 3] = (y + 1) * this->width;		// The first point of next row

			//LOG_INFO(i_pos + 2, this->indexes[i_pos + 2]);
			//LOG_INFO(i_pos + 3, this->indexes[i_pos + 3]);
		}
	}
}



bool AnimatedMesh::Init(const GLchar * vs, const GLchar* fs)
{
	bool status;

	status = Mesh::Init(vs, fs);
	if (!status) return false;

	this->InitPressure();
	this->velocity.x.Reset();
	this->velocity.y.Reset();

	return true;
}



void AnimatedMesh::Update(void)
{
	this->frame_count++;

#ifndef NDEBUG
	if (this->frame_count % 1000 == 0)
	{
		LOG_INFO("FrameCount:", this->frame_count);
	}
#endif  //!NDEBUG

	if (this->frame_count >= this->max_frame_num)
	{
		this->frame_count = 0;

		if (this->reset_data)
		{
			this->InitPressure();
			this->velocity.x.Reset();
			this->velocity.y.Reset();
		}
	}

	double current_time	= GetTime();
	double elapsed_time	= current_time - this->last_time;

	this->last_time = current_time;

//	FOOTPRINT();
#ifndef NDEBUG
	double start_time = GetTime();
#endif //!NDEBUG

	while (elapsed_time > 0.f)
	{
		this->delta_time = elapsed_time > this->MaxDeltaTime ? this->MaxDeltaTime : elapsed_time;
		elapsed_time -= this->delta_time;
		this->ComputePressure();
	}

#ifndef NDEBUG
	double loop_time = GetTime() - start_time;

	if (loop_time > 10 * this->MaxDeltaTime)
	{
		LOG_INFO("LoopTime:", loop_time);
	}
#endif //!NDEBUG

	this->ApplyPressure();
}



void AnimatedMesh::InitPressure()
{
	int cx = this->width / 2;
	int cy = this->height / 2;

	FOOTPRINT();

	// Initialize pressures
	FOR_EACH_VERTEX
	{
		double d = Distance(x, y, cx, cy);

		if (d < cx * 0.2)
		{
			this->pressure(x, y) = -cos(d * ((2 * M_PI) /(double)(this->PressurePeriod)));
		}
		else
		{
			this->pressure(x, y) = 0.0;
		}
	}
}



void AnimatedMesh::ComputePressure(void)
{
    int x, y;
    double time_step = this->delta_time * this->TimeStepMultiple;
    double accel_x = 0;
    double accel_y = 0;

    //FOOTPRINT();

    FOR_EACH_VERTEX
	{
		/** Compute accelerations
		 *  An acceleration is the difference between two adjacent pressures
		 *  along the coordinate axis in X/Y direction.
		 */
		accel_x = this->pressure(x, y) - this->pressure((x + 1) % this->width, y);
		accel_y = this->pressure(x, y) - this->pressure(x, (y + 1) % this->height);

		/** Compute speeds
		 *  a = dv/dt
		 */
		this->velocity.x(x, y) += accel_x * time_step;
		this->velocity.y(x, y) += accel_y * time_step;

		// Compute pressure
		double v;
		if (x >= 1 && y >= 1)
		{
			v = (this->velocity.x(x - 1, y) - this->velocity.x(x, y) + this->velocity.y(x, y - 1) - this->velocity.y(x, y)) * time_step;
		}
		else if (x >= 1)
		{
			v = (this->velocity.x(x - 1, y) - this->velocity.x(x, y)) * time_step;
		}
		else if (y >= 1)
		{
			v = (this->velocity.y(x, y - 1) - this->velocity.y(x, y)) * time_step;
		}
		else // x < 1 && y < 1
		{
			v = (this->velocity.x(x, y) + this->velocity.y(x, y)) * time_step / 2.0;
		}
		this->pressure(x, y) += v;
	}
}



void AnimatedMesh::ApplyPressure(void)
{
    int pos;

    //FOOTPRINT();

#ifndef NDEBUG
    //GLfloat max_z = -1e+10, min_z = 1e+10;
#endif //!NDEBUG

    FOR_EACH_VERTEX
	{
		pos = this->VertexPosition(x, y);

		this->vertices[pos].position.z += (float)this->pressure(x, y) * this->PressureCoefficient;

#ifndef NDEBUG
		//max_z = this->vertices[pos].position.z > max_z ? this->vertices[pos].position.z : max_z;
		//min_z = this->vertices[pos].position.z < min_z ? this->vertices[pos].position.z : min_z;
#endif //!NDEBUG
	}

#ifndef NDEBUG
    //LOG_INFO("MaxZ:", max_z);
    //LOG_INFO("MinZ:", min_z);
#endif //!NDEBUG
}



void FilteringMesh::UpdateZvalue(int x, int y, int pos)
{
#if 0
	float c = 6.f;

    float dx = this->vertices[pos].x * c;
    float dy = this->vertices[pos].y * c;

    this->vertices[pos].z = sinc(dx, dy);
#else
    AnimatedMesh::UpdateZvalue(x, y, pos);
#endif
}



void FilteringMesh::ApplyPressure()
{
    int		pos	= 0;
    float	c 	= 6.f;

    //FOOTPRINT();

    FOR_EACH_VERTEX
	{
    	pos = this->VertexPosition(x, y);

		float dx = this->vertices[pos].position.x * c + (float) (this->pressure(x, y));
		float dy = this->vertices[pos].position.y * c + (float) (this->pressure(x, y));

		this->vertices[pos].position.z = Sinc(dx, dy);
	}
}

