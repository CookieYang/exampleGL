#pragma once
#include "BaseClass.h"

class Tessellation : public baseClass::application {
	void start() {
		tessellationShader = Shader("E://ogl/glExamples/glExamples/shaders/Tessellation/quad.vs", "E://ogl/glExamples/glExamples/shaders/Tessellation/quad.fs", "E://ogl/glExamples/glExamples/shaders/Tessellation/quad.gs", "E://ogl/glExamples/glExamples/shaders/Tessellation/quad.tcs", "E://ogl/glExamples/glExamples/shaders/Tessellation/quad.tes");
		uniform.model_matrix = glGetUniformLocation(tessellationShader.Program, "model_matrix");
		uniform.view_matrix = glGetUniformLocation(tessellationShader.Program, "view_matrix");
		uniform.proj_matrix = glGetUniformLocation(tessellationShader.Program, "proj_matrix");
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f,0.0f,0.0f,1.0f };
		glClearBufferfv(GL_COLOR, 0, black);
		glViewport(0, 0, 1024, 1024);
		vmath::vec3 view_position = vmath::vec3(40.0f, 40.0f, 40.0f);
		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 model_matrix = vmath::translate(vmath::vec3(0.0f, 0.0f, 0.0f)) * vmath::scale(50.0f);
		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)1024.0f / (float)1024.0f, 0.1f, 1000.0f);
		tessellationShader.Use();
		glUniformMatrix4fv(uniform.model_matrix, 1, GL_FALSE, model_matrix);
		glUniformMatrix4fv(uniform.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(uniform.proj_matrix, 1, GL_FALSE, proj_matrix);
		glDrawArrays(GL_PATCHES, 0, 4);
	}
private:
	Shader tessellationShader;
	struct 
	{
		GLuint model_matrix;
		GLuint view_matrix;
		GLuint proj_matrix;
	}uniform;
};