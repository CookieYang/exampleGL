#pragma once
#include "BaseClass.h"

static unsigned int seed = 0x13371337;

static inline unsigned int random_uint()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	return tmp;
}

class Bindless : public baseClass::application {
public:
	void start()
	{
		shader = Shader("shaders/bindless/render.vs", "shaders/bindless/render.fs");
		object.load("resource/torus_nrms_tc.sbm");

		int i;
		int j;

		unsigned char tex_data[32 * 32 * 4];
		unsigned int mutated_data[32 * 32];
		memset(tex_data, 0, sizeof(tex_data));

		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < 32; j++)
			{
				tex_data[i * 4 * 32 + j * 4] = (i ^ j) << 3;
				tex_data[i * 4 * 32 + j * 4 + 1] = (i ^ j) << 3;
				tex_data[i * 4 * 32 + j * 4 + 2] = (i ^ j) << 3;
			}
		}

		glGenBuffers(1, &buffers.transformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers.transformBuffer);

		glBufferStorage(GL_UNIFORM_BUFFER,
			sizeof(MATRICES),
			nullptr,
			GL_MAP_WRITE_BIT);

		glGenBuffers(1, &buffers.textureHandleBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers.textureHandleBuffer);

		glBufferStorage(GL_UNIFORM_BUFFER,
			NUM_TEXTURES * sizeof(GLuint64) * 2,
			nullptr,
			GL_MAP_WRITE_BIT);

		GLuint64* pHandles = (GLuint64*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, NUM_TEXTURES * sizeof(GLuint64) * 2, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for (i = 0; i < NUM_TEXTURES; i++)
		{
			unsigned int r = (random_uint() & 0xFCFF3F) << (random_uint() % 12);
			glGenTextures(1, &textures[i].name);
			glBindTexture(GL_TEXTURE_2D, textures[i].name);
			glTexStorage2D(GL_TEXTURE_2D, TEXTURE_LEVELS, GL_RGBA8, TEXTURE_SIZE, TEXTURE_SIZE);
			for (j = 0; j < 32 * 32; j++)
			{
				mutated_data[j] = (((unsigned int *)tex_data)[j] & r) | 0x20202020;
			}
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_SIZE, TEXTURE_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, mutated_data);
			glGenerateMipmap(GL_TEXTURE_2D);
			textures[i].handle = glGetTextureHandleARB(textures[i].name);
			glMakeTextureHandleResidentARB(textures[i].handle);
			pHandles[i * 2] = textures[i].handle;
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
	void render(double time)
	{
		static double last_time = 0.0;
		static double total_time = 0.0;
		static float black[] = { 0.0,0.0,0.0,1.0 };

		const float f = (float)time;

		int i;

		vmath::mat4 proj_matrix = vmath::perspective(70.0f,
			(float)800.0f / (float)600.0f,
			0.1f, 500.0f);

		glViewport(0, 0, 800.0f, 600.0f);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		glFinish();

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffers.transformBuffer);
		MATRICES* pMatrices = (MATRICES*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(MATRICES), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		pMatrices->view = vmath::translate(0.0f, 0.0f, -80.0f);
		pMatrices->projection = proj_matrix;

		float angle = f;
		float angle2 = 0.7f * f;
		float angle3 = 0.1f * f;
		for (i = 0; i < NUM_TEXTURES; i++)
		{
			pMatrices->model[i] = vmath::translate(float(i % 32) * 4.0f - 62.0f, float(i >> 5) * 6.0f - 33.0f, 15.0f * sinf(angle * 0.19f) + 3.0f * cosf(angle2 * 6.26f) + 40.0f * sinf(angle3)) *
				vmath::rotate(angle * 130.0f, 1.0f, 0.0f, 0.0f) *
				vmath::rotate(angle * 140.0f, 0.0f, 0.0f, 1.0f);
			angle += 1.0f;
			angle2 += 4.1f;
			angle3 += 0.01f;
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		glFinish();

		glBindBufferBase(GL_UNIFORM_BUFFER, 1, buffers.textureHandleBuffer);

		glEnable(GL_DEPTH_TEST);

		shader.Use();

		object.render(NUM_TEXTURES);
	}
private:
	enum
	{
		NUM_TEXTURES = 384,
		TEXTURE_LEVELS = 5,
		TEXTURE_SIZE = (1 << (TEXTURE_LEVELS - 1))
	};

	Shader      shader;

	struct
	{
		GLint   mv_matrix;
		GLint   vp_matrix;
	} uniforms;

	struct
	{
		GLuint      name;
		GLuint64    handle;
	} textures[1024];

	struct
	{
		GLuint      transformBuffer;
		GLuint      textureHandleBuffer;
	} buffers;

	struct MATRICES
	{
		vmath::mat4     view;
		vmath::mat4     projection;
		vmath::mat4     model[NUM_TEXTURES];
	};

	baseClass::object     object;

};