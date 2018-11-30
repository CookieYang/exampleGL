#pragma once
#include "BaseClass.h"
class AttribDivisor : public baseClass::application {
public:
	void start() {
		shader = Shader("shaders/AttribDivisor.vs", "shaders/AttribDivisor.fs");
		static const GLfloat square_vertices[] = {
			-1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, 1.0f, 0.0f, 1.0f
		};

		static const GLfloat instance_colors[] = {
			1.0f,0.0f,0.0f,1.0f,
			0.0f,1.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,1.0f,
			1.0f,1.0f,0.0f,1.0f
		};
		static const GLfloat instance_positions[] = {
			-2.0f,-2.0f,0.0f,0.0f,
			2.0f,-2.0f,0.0f,0.0f,
			2.0f,2.0f,0.0f,0.0f,
			-2.0f,2.0f,0.0f,0.0f
		};

		GLuint offset = 0;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices) + sizeof(instance_colors) + sizeof(instance_positions), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_vertices), square_vertices);
		offset += sizeof(square_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_colors), instance_colors);
		offset += sizeof(instance_colors);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(instance_positions), instance_positions);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(square_vertices));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(square_vertices) + sizeof(instance_colors)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);

	}

	void render(double time) {
		static const GLfloat black[] = {0.0f,0.0f,0.0f,0.0f};
		glClearBufferfv(GL_COLOR, 0, black);
		shader.Use();
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
	}

private:
	Shader shader;
	GLuint VAO;
	GLuint VBO;
};