#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class ShadowMap : public baseClass::application {
public:
	void start() {
		shadowMapShader = Shader("D://ogl/glExamples/glExamples/shaders/shadowMap/shadowMap.vs", "D://ogl/glExamples/glExamples/shaders/shadowMap/shadowMap.fs");
		sceneShader = Shader("D://ogl/glExamples/glExamples/shaders/shadowMap/scene.vs", "D://ogl/glExamples/glExamples/shaders/shadowMap/scene.fs");

		shadowMapUniform.model_matrix = glGetUniformLocation(shadowMapShader.Program, "model_matrix");
		shadowMapUniform.view_matrix = glGetUniformLocation(shadowMapShader.Program, "view_matrix");
		shadowMapUniform.proj_matrix = glGetUniformLocation(shadowMapShader.Program, "proj_matrix");
		shadowMapUniform.obj_color = glGetUniformLocation(shadowMapShader.Program, "obj_color");

		sceneUniform.model_matrix = glGetUniformLocation(sceneShader.Program, "model_matrix");
		sceneUniform.view_matrix = glGetUniformLocation(sceneShader.Program, "view_matrix");
		sceneUniform.proj_matrix = glGetUniformLocation(sceneShader.Program, "proj_matrix");

		torus.load("D://ogl/media/torus_nrms_tc.sbm");
		cube.load("D://ogl/media/cube.sbm");

		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_GREATER);

		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

		testTexture = ktx::load("D://ogl/media/baboon.ktx");

		glGenTextures(1, &shadowTexture);
		glBindTexture(GL_TEXTURE_2D, shadowTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 800, 600);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTexture, 0);

		glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
	}

	void render(double time) {
		static const GLfloat gray[] = { 0.0f, 0.0f, 0.0f, 0.0f };
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
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glViewport(0, 0, 800.0, 600.0);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, gray);
		glDisable(GL_DEPTH_TEST);
		view_position = vmath::vec3(0.0f, 0.0f, 50.0f);
		view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		sceneShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowTexture);
		glUniform1i(glGetUniformLocation(sceneShader.Program, "depthTex"), 0);
		//glUniformMatrix4fv(sceneUniform.model_matrix, 1, GL_FALSE, cube_matrix);
		//glUniformMatrix4fv(sceneUniform.view_matrix, 1, GL_FALSE, view_matrix);
		//glUniformMatrix4fv(sceneUniform.proj_matrix, 1, GL_FALSE, proj_matrix);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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

	struct
	{
		GLuint model_matrix;
		GLuint view_matrix;
		GLuint proj_matrix;
	}sceneUniform;

	Shader sceneShader;
	GLuint shadowTexture;
	GLuint testTexture;
	GLuint shadowFBO;
	baseClass::object torus;
	baseClass::object cube;
};