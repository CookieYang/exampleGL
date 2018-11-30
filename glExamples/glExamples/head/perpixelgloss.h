#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class PerPixelGloss : public baseClass::application {
public:
	void start() {
		shader = Shader("D://ogl/glExamples/glExamples/shaders/perpixelgloss/render.vs", "D://ogl/glExamples/glExamples/shaders/perpixelgloss/render.fs");
		uniforms.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
		glActiveTexture(GL_TEXTURE0);
		tex_envmap = ktx::load("media/envmaps/mountains3d.ktx");
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glActiveTexture(GL_TEXTURE1);
		tex_glossmap = ktx::load("D://ogl/media/pattern1.ktx");
		obj.load("D://ogl/media/torus_nrms_tc.sbm");
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	void render(double time) {
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat ones[] = { 1.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, tex_envmap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_glossmap);

		glViewport(0, 0,800, 600);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(60.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -3.0f) *
			vmath::rotate((float)time * 13.75f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)time * 7.75f, 0.0f, 0.0f, 1.0f) *
			vmath::rotate((float)time * 15.3f, 1.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);

		obj.render();
	}
private:
	Shader shader;
	GLuint tex_envmap;
	GLuint tex_glossmap;
	struct 
	{
		GLint mv_matrix;
		GLint proj_matrix;
	}uniforms;
	baseClass::object obj;
};