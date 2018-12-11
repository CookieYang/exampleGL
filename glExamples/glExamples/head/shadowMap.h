#pragma once
#include "BaseClass.h"

class ShadowMap : public baseClass::application {
public:
	void start() {
		shadowMapShader = Shader("E://ogl/glExamples/glExamples/shaders/shadowMap/shadowMap.vs", "E://ogl/glExamples/glExamples/shaders/shadowMap/shadowMap.fs");
		sceneShader = Shader("E://ogl/glExamples/glExamples/shaders/shadowMap/scene.vs", "E://ogl/glExamples/glExamples/shaders/shadowMap/scene.fs");

		shadowMapUniform.model_matrix = glGetUniformLocation(shadowMapShader.Program, "model_matrix");
		shadowMapUniform.view_matrix = glGetUniformLocation(shadowMapShader.Program, "view_matrix");
		shadowMapUniform.proj_matrix = glGetUniformLocation(shadowMapShader.Program, "proj_matrix");
		shadowMapUniform.obj_color = glGetUniformLocation(shadowMapShader.Program, "obj_color");

		torus.load("E://ogl/media/torus_nrms_tc.sbm");
		cube.load("E://ogl/media/cube.sbm");

		glEnable(GL_DEPTH_TEST);
	}

	void render(double time) {
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat red[] = {1.0f, 0.2f, 0.2f, 1.0f };
		static const GLfloat green[] = { 0.2f, 1.0f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		const float t = (float)time * 0.1f;
		glViewport(0, 0, 800, 600);

		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)800.0f / (float)600.0f, 0.1f, 1000.0f);
		vmath::vec3 view_position = vmath::vec3(0.0f , 2.0f, 30.0f );
		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 cube_matrix = vmath::translate(vmath::vec3(0.0f, 0.0f, -100.0f)) * vmath::scale(50.0f);

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		shadowMapShader.Use();
		glUniformMatrix4fv(shadowMapUniform.model_matrix, 1, GL_FALSE, cube_matrix);
		glUniformMatrix4fv(shadowMapUniform.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(shadowMapUniform.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniform4fv(shadowMapUniform.obj_color, 1, red);
		cube.render();

		vmath::mat4 torus_matrix = vmath::translate(vmath::vec3(0.0f, 0.0f, 10.0f)) * vmath::scale(5.0f);
		glUniformMatrix4fv(shadowMapUniform.model_matrix, 1, GL_FALSE, torus_matrix);
		glUniform4fv(shadowMapUniform.obj_color, 1, green);
		torus.render();
	}

private:
	Shader shadowMapShader;
	struct 
	{
		GLuint model_matrix;
		GLuint view_matrix;
		GLuint proj_matrix;
		GLuint obj_color;
	}shadowMapUniform;
	Shader sceneShader;
	GLuint shadowTexture;
	baseClass::object torus;
	baseClass::object cube;
};