#pragma once
#include "BaseClass.h"
class BasicFBO : public baseClass::application {
public:
	void start() {
		shader1 = Shader("shaders/BasicFBO/BasicFBO.vs", "shaders/BasicFBO/BasicFBO1.fs");
		shader2 = Shader("shaders/BasicFBO/BasicFBO.vs", "shaders/BasicFBO/BasicFBO2.fs");

		mv_location = glGetUniformLocation(shader1.Program, "mv_matrix");
		proj_location = glGetUniformLocation(shader1.Program, "proj_matrix");
		mv_location2 = glGetUniformLocation(shader2.Program, "mv_matrix");
		proj_location2 = glGetUniformLocation(shader2.Program, "proj_matrix");

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		static const GLushort vertex_indices[] =
		{
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			6, 0, 2,
			2, 4, 6,
			7, 5, 3,
			7, 3, 1
		};

		static const GLfloat vertex_data[] =
		{
			// Position                 Tex Coord
			-0.25f, -0.25f,  0.25f,      0.0f, 1.0f,
			-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
			0.25f, -0.25f, -0.25f,      1.0f, 0.0f,

			0.25f, -0.25f, -0.25f,      1.0f, 0.0f,
			0.25f, -0.25f,  0.25f,      1.0f, 1.0f,
			-0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

			0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
			0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
			0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

			0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
			0.25f,  0.25f,  0.25f,      1.0f, 1.0f,
			0.25f, -0.25f,  0.25f,      0.0f, 1.0f,

			0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
			-0.25f,  0.25f, -0.25f,      0.0f, 0.0f,
			0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

			-0.25f,  0.25f, -0.25f,      0.0f, 0.0f,
			-0.25f,  0.25f,  0.25f,      0.0f, 1.0f,
			0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

			-0.25f,  0.25f, -0.25f,      1.0f, 0.0f,
			-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
			-0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

			-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
			-0.25f, -0.25f,  0.25f,      0.0f, 1.0f,
			-0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

			-0.25f,  0.25f, -0.25f,      0.0f, 1.0f,
			0.25f,  0.25f, -0.25f,      1.0f, 1.0f,
			0.25f, -0.25f, -0.25f,      1.0f, 0.0f,

			0.25f, -0.25f, -0.25f,      1.0f, 0.0f,
			-0.25f, -0.25f, -0.25f,      0.0f, 0.0f,
			-0.25f,  0.25f, -0.25f,      0.0f, 1.0f,

			-0.25f, -0.25f,  0.25f,      0.0f, 0.0f,
			0.25f, -0.25f,  0.25f,      1.0f, 0.0f,
			0.25f,  0.25f,  0.25f,      1.0f, 1.0f,

			0.25f,  0.25f,  0.25f,      1.0f, 1.0f,
			-0.25f,  0.25f,  0.25f,      0.0f, 1.0f,
			-0.25f, -0.25f,  0.25f,      0.0f, 0.0f,
		};

		glGenBuffers(1, &position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(vertex_data),
			vertex_data,
			GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(vertex_indices),
			vertex_indices,
			GL_STATIC_DRAW);

		glEnable(GL_CULL_FACE);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		
		glGenTextures(1, &color_texture);
		glBindTexture(GL_TEXTURE_2D, color_texture);
		glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 512, 512);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenTextures(1, &depth_texture);
		glBindTexture(GL_TEXTURE_2D, depth_texture);
		glTexStorage2D(GL_TEXTURE_2D, 9, GL_DEPTH_COMPONENT32F, 512, 512);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

		static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_buffers);
		
	}

	void render(double time) {
		static const GLfloat blue[] = { 0.0f, 0.0f, 0.3f, 1.0f };
		static const GLfloat green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;

		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0f / (float)600.0f,
			0.1f,
			1000.0f);

		float f = (float)time * 0.3f;
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::translate(sinf(2.1f * f) * 0.5f,
				cosf(1.7f * f) * 0.5f,
				sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
			vmath::rotate((float)time * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)time * 81.0f, 1.0f, 0.0f, 0.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glViewport(0, 0, 512, 512);
		glClearBufferfv(GL_COLOR, 0, green);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		shader1.Use();

		glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 800.0, 600.0);
		glClearBufferfv(GL_COLOR, 0, blue);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glBindTexture(GL_TEXTURE_2D, color_texture);

		shader2.Use();

		glUniformMatrix4fv(proj_location2, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(mv_location2, 1, GL_FALSE, mv_matrix);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	Shader shader1;
	Shader shader2;

	GLuint          vao;
	GLuint          position_buffer;
	GLuint          index_buffer;
	GLuint          fbo;
	GLuint          color_texture;
	GLuint          depth_texture;
	GLint           mv_location;
	GLint           proj_location;
	GLuint          mv_location2;
	GLuint          proj_location2;
};