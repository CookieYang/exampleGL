#pragma once
#include "BaseClass.h"

class Gooch : public baseClass::application {
	void start() {

		goochShader = Shader("E://ogl/glExamples/glExamples/shaders/Gooch/gooch.vs", "E://ogl/glExamples/glExamples/shaders/Gooch/gooch.fs");

		uniform.diffuse_albedo = glGetUniformLocation(goochShader.Program, "diffuse_albedo");
		uniform.specular_albedo = glGetUniformLocation(goochShader.Program, "specular_albedo");
		uniform.specular_power = glGetUniformLocation(goochShader.Program, "specular_power");

		glGenBuffers(1, &uniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniformBlock), NULL, GL_DYNAMIC_DRAW);

		obj.load("E://ogl/media/sphere.sbm");

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void render(double time) {

		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };

		goochShader.Use();

		glViewport(0.0, 0.0, 800, 600);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 50.0f);

		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBuffer);
		uniformBlock* block = (uniformBlock*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(uniformBlock), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		vmath::mat4 model_matrix = vmath::scale(7.0f);

		block->mvMatrix = view_matrix * model_matrix;
		block->viewMatrix = view_matrix;
		block->projMatrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		glUniform1f(uniform.specular_power, 30.0f);
		glUniform3fv(uniform.specular_albedo, 1, vmath::vec3(1.0f));

		obj.render();
	}

private:

	Shader goochShader;

	struct {
		GLint diffuse_albedo;
		GLint specular_albedo;
		GLint specular_power;
	}uniform;

	GLuint uniformBuffer;

	struct uniformBlock
	{
		vmath::mat4 mvMatrix;
		vmath::mat4 viewMatrix;
		vmath::mat4 projMatrix;
	};

	baseClass::object obj;
};