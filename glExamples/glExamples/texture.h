#pragma once
#include "soil.h"
#include "KTXLoader.h"
#include "BaseClass.h"

class Texture : public baseClass::application {
	void start() {
		shader = Shader("shaders/texture.vs", "shaders/texture.fs");
		shader.Use();
		GLfloat vertices[] = {
		0.0,0.0,0.0,
		0.5,0.0,0.0,
		0.5,0.5,0.0
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);

		/*int width, height, channels;
		unsigned char* data = SOIL_load_image("resource/texture.png", &width, &height, &channels, 0);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTextureImage2DEXT(texture, GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(data);
		glBindTexture(GL_TEXTURE_2D, 0);*/

		glGenTextures(1, &texture);
		ktx::load("resource/brick.ktx", texture);
	}

	void render(double time) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader.Program, "texture"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	Shader shader;
	GLuint VAO;
	GLuint VBO;
	GLuint texture;
};