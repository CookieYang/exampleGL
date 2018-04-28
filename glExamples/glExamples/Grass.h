#pragma once
#include "KTXLoader.h"
#include "BaseClass.h"

class Grass : public baseClass::application {
public:
	void start() {
		static const GLfloat grass_blade[] =
		{
			-0.3f, 0.0f,
			0.3f, 0.0f,
			-0.20f, 1.0f,
			0.1f, 1.3f,
			-0.05f, 2.3f,
			0.0f, 3.3f
		};

		shader = Shader("shaders/instanceGrass.vs", "shaders/instanceGrass.fs");

		glGenVertexArrays(1, &grass_vao);
		glBindVertexArray(grass_vao);
		
		glGenBuffers(1, &grass_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, grass_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(grass_blade), grass_blade, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(0);

		glActiveTexture(GL_TEXTURE1);
		tex_grass_length = ktx::load("resource/grass_length.ktx");
		glActiveTexture(GL_TEXTURE2);
		tex_grass_orientation = ktx::load("resource/grass_orientation.ktx");
		glActiveTexture(GL_TEXTURE3);
		tex_grass_color = ktx::load("resource/grass_color.ktx");
		glActiveTexture(GL_TEXTURE4);
		tex_grass_bend = ktx::load("resource/grass_bend.ktx");

		shader.Use();
		uniforms.mvpMatrix = glGetUniformLocation(shader.Program, "mvpMatrix");
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void render(double time) {
		float t = (float)time * 0.02f;
		float r = 550.0f;

		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		vmath::mat4 mv_matrix = vmath::lookat(vmath::vec3(sinf(t) * r, 25.0f, cosf(t) * r),
			vmath::vec3(0.0f, -50.0f, 0.0f),
			vmath::vec3(0.0, 1.0, 0.0));
		vmath::mat4 prj_matrix = vmath::perspective(45.0f, (float)800.0 / (float)600.0, 0.1f, 1000.0f);

		vmath::mat4 transform = vmath::translate(0.0f,0.0f,0.0f) * vmath::scale(1.0f,1.0f,1.0f);
	
		glUniformMatrix4fv(uniforms.mvpMatrix, 1, GL_FALSE, (prj_matrix * mv_matrix  ));

		glBindVertexArray(grass_vao);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 512 * 512);
	}

private:
	Shader shader;
	GLuint grass_vao;
	GLuint grass_vbo;

	GLuint      tex_grass_color;
	GLuint      tex_grass_length;
	GLuint      tex_grass_orientation;
	GLuint      tex_grass_bend;

	struct
	{
		GLint   mvpMatrix;
	} uniforms;
};