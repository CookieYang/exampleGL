#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class Equirectangle :public baseClass::application {
public:
	void start() {
		envmap_index = 0; 
		shader = Shader("shaders/equirectangular/render.vs", "shaders/equirectangular/render.fs");
		uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");

		envmap[0] = ktx::load("media/envmaps/equirectangularmap1.ktx");
		tex_envmap = envmap[envmap_index];
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		obj.load("media/dragon.sbm");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

	}
	void render(double time) {
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		glBindTexture(GL_TEXTURE_2D, tex_envmap);

		glViewport(0, 0, 800, 600);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)800.0 / (float)600.0, 0.1f, 1000.0f);
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -15.0f) *
			vmath::rotate((float)time, 1.0f, 0.0f, 0.0f) *
			vmath::rotate((float)time * 1.1f, 0.0f, 1.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);

		obj.render();
	}
private:
	Shader shader;
	GLuint tex_envmap;
	GLuint envmap[3];
	int envmap_index;
	struct
	{
		GLint mv_matrix;
		GLint proj_matrix;
	}uniforms;
	baseClass::object obj;
};
