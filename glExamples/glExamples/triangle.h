#pragma once
#include "BaseClass.h"
class TriangleInstance : public baseClass::application {
public:
	void start() {
		shader = Shader("shaders/triangle.vs", "shaders/triangle.fs", nullptr, "shaders/triangle.tcs", "shaders/triangle.tes");
		shader.Use();
		//glPointSize(40.0f);
		//glGenVertexArrays(1, &VAO);
		//glBindVertexArray(VAO);
		static const GLfloat vertices[] = {
			-0.25f, 0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,
			/* MORE DATA HERE */
			-0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, 0.25f,
			0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, -0.25f
		};
		//glGenBuffers(1, &VBO);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
		//glEnableVertexAttribArray(0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void render(double time) {
		//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
		const GLfloat color[] = { 0.0f,0.0f,0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, color);
		glDrawArrays(GL_PATCHES, 0, 3);
	}

private:
	Shader shader;
	GLuint VAO;
	GLuint VBO;
};