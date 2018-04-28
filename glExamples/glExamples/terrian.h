#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class Terrian : public baseClass::application{
public:
	void start() {
		shader = Shader("shaders/terrian/terrian.vs", "shaders/terrian/terrian.fs", nullptr, "shaders/terrian/terrian.tcs", "shaders/terrian/terrian.tes");
		uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
		uniforms.mvp_matrix = glGetUniformLocation(shader.Program, "mvp_matrix");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
		uniforms.dmap_depth = glGetUniformLocation(shader.Program, "dmap_depth");
		uniforms.enable_fog = glGetUniformLocation(shader.Program, "enable_fog");
		dmap_depth = 6.0f;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glEnable(GL_CULL_FACE);

		glGenTextures(1, &tex_displacement);
		ktx::load("resource/terragen1.ktx",tex_displacement);
		glGenTextures(1, &tex_color);
		ktx::load("resource/terragen_color.ktx",tex_color);
	}

	void render(double time) {
		static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0;
		static double total_time = 0.0;

	
		total_time += (time - last_time);
		last_time = time;

		float t = (float)total_time * 0.03f;
		float r = sinf(t * 5.37f) * 15.0f + 16.0f;
		float h = cosf(t * 4.79f) * 2.0f + 3.2f;

		glViewport(0, 0, 800.0, 600.0);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		vmath::mat4 mv_matrix = /* vmath::translate(0.0f, 0.0f, -1.4f) *
								vmath::translate(0.0f, -0.4f, 0.0f) * */
								// vmath::rotate((float)currentTime * 6.0f, 0.0f, 1.0f, 0.0f) *
		vmath::lookat(vmath::vec3(sinf(t) * r, h, cosf(t) * r), vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 proj_matrix = vmath::perspective(60.0f,
			(float)800.0 / (float)600.0,
			0.1f, 1000.0f);

		shader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_displacement);
		glUniform1i(glGetUniformLocation(shader.Program, "tex_displacement"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_color);
		glUniform1i(glGetUniformLocation(shader.Program, "tex_color"), 1);

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(uniforms.mvp_matrix, 1, GL_FALSE, proj_matrix * mv_matrix);
		glUniform1f(uniforms.dmap_depth,  dmap_depth);
		glUniform1i(uniforms.enable_fog, 1);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
		glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
	}

private:
	Shader shader;
	float dmap_depth;
	GLuint vao;
	GLuint tex_displacement;
	GLuint tex_color;

	struct {
		GLint       mvp_matrix;
		GLint       mv_matrix;
		GLint       proj_matrix;
		GLint       dmap_depth;
		GLint       enable_fog;
	} uniforms;
};