#pragma once
#include "BaseClass.h"
class Phong : public baseClass::application {
public:
	void start() {
		per_fragment = true;

		gouraud = Shader("shaders/ponglighting/gouraud.vs", "shaders/ponglighting/gouraud.fs");
		pong = Shader("shaders/ponglighting/pong.vs", "shaders/ponglighting/pong.fs");
		uniforms[0].diffuse_albedo = glGetUniformLocation(gouraud.Program, "diffuse_albedo");
		uniforms[0].specular_albedo = glGetUniformLocation(gouraud.Program, "specular_albedo");
		uniforms[0].specular_power = glGetUniformLocation(gouraud.Program, "specular_power");
		uniforms[1].diffuse_albedo = glGetUniformLocation(pong.Program, "diffuse_albedo");
		uniforms[1].specular_albedo = glGetUniformLocation(pong.Program, "specular_albedo");
		uniforms[1].specular_power = glGetUniformLocation(pong.Program, "specular_power");

		glGenBuffers(1, &uniforms_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

		obj.load("media/sphere.sbm");

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	void render(double time) {
		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };
		const float f = (float)time;
		if (!per_fragment)
		{
			gouraud.Use();
		}
		else
		{
			pong.Use();
		}
		
		glViewport(0.0, 0.0, 800, 600);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

		vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 50.0f);
		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
		uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
			0,
			sizeof(uniforms_block),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		vmath::mat4 model_matrix = vmath::scale(7.0f);

		block->mv_matrix = view_matrix * model_matrix;
		block->view_matrix = view_matrix;
		block->proj_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		if (!per_fragment)
		{
			glUniform1f(uniforms[0].specular_power, 30.0f);
			glUniform3fv(uniforms[0].specular_albedo, 1, vmath::vec3(1.0f));
		}
		else
		{
			glUniform1f(uniforms[1].specular_power, 30.0f);
			glUniform3fv(uniforms[1].specular_albedo, 1, vmath::vec3(1.0f));
		}

		obj.render();
	}
private:
	Shader gouraud;
	Shader pong;
	struct
	{
		GLuint      color;
		GLuint      normals;
	} textures;

	struct uniforms_block
	{
		vmath::mat4     mv_matrix;
		vmath::mat4     view_matrix;
		vmath::mat4     proj_matrix;
	};

	GLuint          uniforms_buffer;

	struct
	{
		GLint           diffuse_albedo;
		GLint           specular_albedo;
		GLint           specular_power;
	} uniforms[2];

	baseClass::object     obj;
	bool per_fragment;
};