#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class WrapMode : public baseClass::application {
	void start() {
		glGenTextures(1, &tex);
		ktx::load("resource/rightarrows.ktx", tex);
		shader = Shader("shaders/warpmode.vs", "shaders/warpmode.fs");
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, tex);
	}

	void render(double time) {
		static const GLfloat green[] = { 0.0f, 0.1f, 0.0f, 1.0f };
		static const GLfloat yellow[] = { 0.4f, 0.4f, 0.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, green);

		static const GLenum wrapmodes[] = { GL_CLAMP_TO_EDGE, GL_REPEAT, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT };
		static const float offsets[] = { -0.5f, -0.5f,
			0.5f, -0.5f,
			-0.5f,  0.5f,
			0.5f,  0.5f };

		shader.Use();
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, yellow);

		for (int i = 0; i < 4; i++)
		{
			glUniform2fv(0, 1, &offsets[i * 2]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapmodes[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapmodes[i]);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

private:
	Shader shader;
	GLuint VAO;
	GLuint tex;
};