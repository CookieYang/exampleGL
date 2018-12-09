#pragma once
#include "BaseClass.h"

class Geometry : public baseClass::application
{
public:
	void start() {
		sceneShader = Shader("E://ogl/glExamples/glExamples/shaders/Geometry/geometry.vs", "E://ogl/glExamples/glExamples/shaders/Geometry/geometry.fs","E://ogl/glExamples/glExamples/shaders/Geometry/geometry.gs");
		matrixUniform.model_matrix = glGetUniformLocation(sceneShader.Program, "model_matrix");
		matrixUniform.proj_matrix = glGetUniformLocation(sceneShader.Program, "proj_matrix");
		matrixUniform.view_matrix = glGetUniformLocation(sceneShader.Program, "view_matrix");
		light_position = glGetUniformLocation(sceneShader.Program, "light_position");
		obj.load("E://ogl/media/sphere.sbm");

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
	}
	void render(double time) {
		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };
		const float f = (float)time;

		vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 50.0f);
		vmath::vec4 lightPosition = vmath::vec4(20.0f, 20.0f, 20.0f, 1.0f);

		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		
		vmath::mat4 model_matrix = vmath::scale(1.0f);

		vmath::mat4 project_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		sceneShader.Use();

		glViewport(0.0, 0.0, 800, 600);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		glUniformMatrix4fv(matrixUniform.model_matrix, 1, GL_FALSE, model_matrix);
		glUniformMatrix4fv(matrixUniform.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(matrixUniform.proj_matrix, 1, GL_FALSE, project_matrix);
		glUniform4fv(light_position, 1, lightPosition);
		obj.render();
	}
private:
	struct 
	{
		GLuint model_matrix;
		GLuint view_matrix;
		GLuint proj_matrix;
	}matrixUniform;
	GLuint light_position;


	Shader sceneShader;
	baseClass::object obj;
};

