#pragma once
#include "BaseClass.h"
class ClipDistance : public baseClass::application {
public:
	void start() {
		object.load("resource/dragon.sbm");
		shader = Shader("shaders/clipDistance/render.vs", "shaders/clipDistance/render.fs");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
		uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
		uniforms.clip_plane = glGetUniformLocation(shader.Program, "clip_plane");
		uniforms.clip_sphere = glGetUniformLocation(shader.Program, "clip_sphere");
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f,0.0f,0.0f,0.0f };
		static const GLfloat one = 1.0f;
		
		static double last_time = 0.0;
		static double total_time = 0.0;
		total_time += (time - last_time);
		last_time = time;

		float f = (float)total_time;

		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -15.0f) *
			vmath::rotate(f * 0.34f, 0.0f, 1.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		vmath::mat4 plane_matrix = vmath::rotate(f * 6.0f, 1.0f, 0.0f, 0.0f) *
			vmath::rotate(f * 7.3f, 0.0f, 1.0f, 0.0f);

		vmath::vec4 plane = plane_matrix[0];
		plane[3] = 0.0f;
		plane = vmath::normalize(plane);

		vmath::vec4 clip_sphere = vmath::vec4(sinf(f * 0.7f) * 3.0f, cosf(f * 1.9f) * 3.0f, sinf(f * 0.1f) * 3.0f, cosf(f * 1.7f) + 2.5f);

		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniform4fv(uniforms.clip_plane, 1, plane);
		glUniform4fv(uniforms.clip_sphere, 1, clip_sphere);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CLIP_DISTANCE0);
		glEnable(GL_CLIP_DISTANCE1);

		object.render();
	}
private:
	baseClass::object object;
	Shader shader;
	struct 
	{
		GLint proj_matrix;
		GLint mv_matrix;
		GLint clip_plane;
		GLint clip_sphere;
	} uniforms;
};