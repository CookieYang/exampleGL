#pragma once

#include "BaseClass.h"
#include "KTXLoader.h"

class ReflectAndRefract : public baseClass::application {
public:
	void start() {
		objShader = Shader("E://ogl/glExamples/glExamples/shaders/ReflectAndRefract/glassObj.vs", "E://ogl/glExamples/glExamples/shaders/ReflectAndRefract/glassObj.fs");
		envShader = Shader("E://ogl/glExamples/glExamples/shaders/ReflectAndRefract/env.vs", "E://ogl/glExamples/glExamples/shaders/ReflectAndRefract/env.fs");

		envUniform.model_matrix = glGetUniformLocation(envShader.Program, "model_matrix");
		envUniform.view_matrix = glGetUniformLocation(envShader.Program, "view_matrix");
		envUniform.proj_matrix = glGetUniformLocation(envShader.Program, "proj_matrix");

		objUniform.model_matrix = glGetUniformLocation(objShader.Program, "model_matrix");
		objUniform.view_matrix = glGetUniformLocation(objShader.Program, "view_matrix");
		objUniform.proj_matrix = glGetUniformLocation(objShader.Program, "proj_matrix");
		objUniform.view_position = glGetUniformLocation(objShader.Program, "view_position");
		envObj.load("E://ogl/media/sphere.sbm");
		//glassObj.load("E://ogl/media/torus_nrms_tc.sbm");
		glassObj.load("E://ogl/media/sphere.sbm");
		cubemap = ktx::load("E://ogl/media/envmaps/mountaincube.ktx");
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
	}

	void render(double time) {
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		const float t = (float)time * 0.1f;
		glViewport(0, 0, 800, 600);

		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)800.0f / (float)600.0f, 0.1f, 1000.0f);
		vmath::vec3 view_position = vmath::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t));
		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 model_matrix = vmath::scale(12.0f);

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		

		
		envShader.Use();
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glUniformMatrix4fv(envUniform.model_matrix, 1, GL_FALSE, model_matrix);
		glUniformMatrix4fv(envUniform.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(envUniform.proj_matrix, 1, GL_FALSE, proj_matrix);
		glFrontFace(GL_CW);
		envObj.render();

		model_matrix = vmath::scale(3.0f);
		vmath::vec4 vp = vmath::vec4(view_position, 1.0f);
		objShader.Use();
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glUniformMatrix4fv(objUniform.model_matrix, 1, GL_FALSE, model_matrix);
		glUniformMatrix4fv(objUniform.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(objUniform.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniform4fv(objUniform.view_position, 1, vp);
		glFrontFace(GL_CCW);
		glassObj.render();
	}
private:
	Shader envShader;
	Shader objShader;
	baseClass::object envObj;
	baseClass::object glassObj;
	struct 
	{
		GLuint model_matrix;
		GLuint view_matrix;
		GLuint proj_matrix;
	}envUniform;
	struct 
	{
		GLuint model_matrix;
		GLuint view_matrix;
		GLuint proj_matrix;
		GLuint view_position;
	}objUniform;
	GLuint cubemap;
};