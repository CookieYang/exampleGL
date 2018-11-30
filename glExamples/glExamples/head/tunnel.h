#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class Tunnel : public baseClass::application {
	void start() {
		shader = Shader("shaders/tunnel.vs", "shaders/tunnel.fs");
		uniforms.mvp = glGetUniformLocation(shader.Program, "mvp");
		uniforms.offset = glGetUniformLocation(shader.Program, "offset");

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		tex_wall = ktx::load("resource/brick.ktx");
		tex_floor = ktx::load("resource/floor.ktx");
		tex_ceiling = ktx::load("resource/ceilling.ktx");

		GLuint textures[3] = { tex_floor, tex_wall, tex_ceiling };
		for (size_t i = 0; i < 3; i++)
		{
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float t = (float)time;

		glViewport(0, 0, 800,600);
		glClearBufferfv(GL_COLOR, 0, black);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(60.0f,
			(float)800 / (float)600,
			0.1f, 100.0f);

		glUniform1f(uniforms.offset, t * 0.003f);

		int i;
		GLuint textures[] = { tex_wall, tex_floor, tex_wall, tex_ceiling };
		for (i = 0; i < 4; i++)
		{
			vmath::mat4 mv_matrix = vmath::rotate(90.0f * (float)i, vmath::vec3(0.0f, 0.0f, 1.0f)) *
				vmath::translate(-0.5f, 0.0f, -10.0f) *
				vmath::rotate(90.0f, 0.0f, 1.0f, 0.0f) *
				vmath::scale(30.0f, 1.0f, 1.0f);
			vmath::mat4 mvp = proj_matrix * mv_matrix;

			glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, mvp);

			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

private:
	Shader shader;
	GLuint VAO;

	struct
	{
		GLint mvp;
		GLint offset;
	} uniforms;

	GLuint tex_wall;
	GLuint tex_ceiling;
	GLuint tex_floor;
};