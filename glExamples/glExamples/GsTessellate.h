#pragma once
#include "BaseClass.h"

class GSTessllate : public baseClass::application {
public:

	void start() {
		shader = Shader("shaders/GSTessellate/GSTessellate.vs", "shaders/GSTessellate/GSTessellate.fs", "shaders/GSTessellate/GSTessellate.gs");

		static const GLfloat tetrahedron_verts[] =
		{
			0.000f,  0.000f,  1.000f,
			0.943f,  0.000f, -0.333f,
			-0.471f,  0.816f, -0.333f,
			-0.471f, -0.816f, -0.333f
		};

		static const GLushort tetrahedron_indices[] =
		{
			0, 1, 2,
			0, 2, 3,
			0, 3, 1,
			3, 2, 1
		};

		mv_location = glGetUniformLocation(shader.Program, "mvMatrix");
		mvp_location = glGetUniformLocation(shader.Program, "mvpMatrix");
		stretch_location = glGetUniformLocation(shader.Program, "stretch");

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedron_verts) + sizeof(tetrahedron_indices), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(tetrahedron_indices), tetrahedron_indices);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedron_indices), sizeof(tetrahedron_verts), tetrahedron_verts);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(tetrahedron_indices));
		glEnableVertexAttribArray(0);

		glEnable(GL_CULL_FACE);
		// glDisable(GL_CULL_FACE);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		float f = (float)time;

		glViewport(0, 0,800.0f, 600.0f);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0f / (float)600.0f,
			0.1f,
			1000.0f);
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -8.0f) *
			vmath::rotate((float)time * 71.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)time * 10.0f, 1.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, proj_matrix * mv_matrix);

		glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);

		glUniform1f(stretch_location, sinf(f * 4.0f) * 0.75f + 1.0f);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, NULL);
	}

private:
	Shader shader;

	GLint           mv_location;
	GLint           mvp_location;
	GLint           stretch_location;
	GLuint          vao;
	GLuint          buffer;
};