#pragma once
#pragma once
#include "BaseClass.h"
class SpinningCube : public baseClass::application {
public:
	void start() {
		shader = Shader("shaders/spinningcube.vs", "shaders/spinningcube.fs");
		shader.Use();
		glPointSize(40.0f);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		static const GLfloat vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
	}

	void render(double time) {
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		vmath::mat4 proj_matrix = vmath::perspective(50.0f, (float)800.0 / 600.0f, 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "proj_matrix"), 1, GL_FALSE, proj_matrix);
		for (int i = 0; i < 24; i++)
		{
			// Calculate a new model-view matrix for each one
			float f = (float)i + (float)time * 0.3f;
			vmath::mat4 mv_matrix =
				vmath::translate(0.0f, 0.0f, -20.0f) *
				vmath::rotate((float)time * 45.0f, 0.0f, 1.0f, 0.0f) *
				vmath::rotate((float)time * 21.0f, 1.0f, 0.0f, 0.0f) *
				vmath::translate(sinf(2.1f * f) * 2.0f,
					cosf(1.7f * f) * 2.0f,
					sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
			// Update the uniform
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "mv_matrix"), 1, GL_FALSE, mv_matrix);
			// Draw - notice that we haven't updated the projection matrix
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
	}

private:
	Shader shader;
	GLuint VAO;
	GLuint VBO;
};