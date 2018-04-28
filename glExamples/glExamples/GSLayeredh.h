#pragma once
#include "BaseClass.h"
class GSLayered : public baseClass::application {
public:
	void start() {
		showShader = Shader("Shaders/GSLayered/showLayers.vs", "Shaders/GSLayered/showLayers.fs");
		gsShader = Shader("Shaders/GSLayered/gsLayers.vs", "Shaders/GSLayered/gsLayers.fs", "Shaders/GSLayered/gsLayers.gs");

		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		obj.load("resource/torus.sbm");

		glGenBuffers(1, &transform_ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, transform_ubo);

		glBufferData(GL_UNIFORM_BUFFER, 17 * sizeof(vmath::mat4), NULL, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &array_texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, array_texture);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 16);

		glGenBuffers(1, &array_depth);
		glBindTexture(GL_TEXTURE_2D_ARRAY, array_depth);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, 256, 256, 16);

		glGenFramebuffers(1, &layered_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, layered_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, array_texture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, array_depth, 0);
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat one = 1.0f;

		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::rotate((float)time * 5.0f, 0.0f, 0.0f, 1.0f) *
			vmath::rotate((float)time * 30.0f, 1.0f, 0.0f, 0.0f);
		vmath::mat4 proj_matrix = vmath::perspective(50.0f, (float)800.0f / (float)600.0f, 0.1f, 1000.0f);
		vmath::mat4 mvp = proj_matrix * mv_matrix;

		struct TRANSFORM_BUFFER
		{
			vmath::mat4 proj_matrix;
			vmath::mat4 mv_matrix[16];
		};

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, transform_ubo);
		TRANSFORM_BUFFER* buffer = (TRANSFORM_BUFFER*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(transform_ubo), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		buffer->proj_matrix = vmath::perspective(50.0f, 1.0f, 0.1f, 1000.0f); // proj_matrix;
		int i;

		for (i = 0; i < 16; i++)
		{
			float fi = (float)(i + 12) / 16.0f;
			buffer->mv_matrix[i] = vmath::translate(0.0f, 0.0f, -4.0f) *
				vmath::rotate((float)time * 25.0f * fi, 0.0f, 0.0f, 1.0f) *
				vmath::rotate((float)time * 30.0f * fi, 1.0f, 0.0f, 0.0f);
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		static const GLenum ca0 = GL_COLOR_ATTACHMENT0;
		glBindFramebuffer(GL_FRAMEBUFFER, layered_fbo);
		glDrawBuffers(1, &ca0);

		glViewport(0, 0, 256, 256);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		gsShader.Use();
		obj.render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);

		glViewport(0, 0, 800.0f, 600.0f);
		glClearBufferfv(GL_COLOR, 0, gray);

		showShader.Use();
		glBindTexture(GL_TEXTURE_2D_ARRAY, array_texture);
		glDisable(GL_DEPTH_TEST);

		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 16);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

private:
	Shader showShader;
	Shader gsShader;

	GLuint vao;
	GLuint transform_ubo;

	GLuint array_texture;
	GLuint array_depth;
	GLuint layered_fbo;
	baseClass::object obj;
};