#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"
class NormalMapping :public baseClass::application {
public:
	void start() {

		shader = Shader("shaders/bumpMapping/render.vs", "shaders/bumpMapping/render.fs");

		uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
		uniforms.light_pos = glGetUniformLocation(shader.Program, "light_pos");

		glActiveTexture(GL_TEXTURE0);
		texture.color = ktx::load("media/ladybug_co.ktx");
		glActiveTexture(GL_TEXTURE1);
		texture.normals = ktx::load("media/ladybug_nm.ktx");

		obj.load("media/ladybug.sbm");
	}
	void render(double time) {
		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };
		static double last_time = 0.0;
		static double total_time = 0.0;

		total_time += (time - last_time);
		last_time = time;

		const float f = (float)total_time;

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glViewport(0, 0,800, 600);
		glEnable(GL_DEPTH_TEST);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(50.0f,(float)800.0 / (float)600.0,0.1f,1000.0f);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);

		vmath::mat4 mv_matrix = vmath::translate(0.0f, -0.2f, -5.5f) *
			vmath::rotate(14.5f, 1.0f, 0.0f, 0.0f) *
			vmath::rotate(-20.0f, 0.0f, 1.0f, 0.0f) *
			vmath::mat4::identity();
		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);

		glUniform3fv(uniforms.light_pos, 1, vmath::vec3(40.0f * sinf(f), 30.0f + 20.0f * cosf(f), 40.0f));

		obj.render();
	}
private:
	Shader shader;
	struct 
	{
		GLuint color;
		GLuint normals;
	}texture;
	struct 
	{
		GLint mv_matrix;
		GLint proj_matrix;
		GLint light_pos;
	}uniforms;

	baseClass::object obj;
};