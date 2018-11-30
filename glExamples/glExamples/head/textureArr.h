#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"
#include <cmath>

static unsigned int seed = 0x13371337;

static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

class TextureArr : public baseClass::application {
	void start() {
		const GLfloat vertices[] = {-0.5,-0.5,0.5,-0.5,-0.5,0.5,0.5,0.5};
		shader = Shader("shaders/textureArr.vs", "shaders/textureArr.fs");
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		tex_alien_arr = ktx::load("resource/aliens.ktx");
		glBindTexture(GL_TEXTURE_2D_ARRAY, tex_alien_arr);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenBuffers(1, &rain_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, rain_buffer);
		glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(vmath::mat4), nullptr, GL_DYNAMIC_DRAW);

		for (size_t i = 0; i < 256; i++)
		{
			droplet_x_offset[i] = random_float() * 2.0f - 1.0f;
			droplet_rot_speed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
			droplet_fall_speed[i] = random_float() + 0.2f;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void render(double time) {
		static const GLfloat black[] = {0.0,0.0,0.0,0.0};
		glViewport(0, 0, 800, 600);
		glClearBufferfv(GL_COLOR, 0, black);

		shader.Use();

		// ¸ø uniformbufferobjectÌî³äÊý¾Ý
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, rain_buffer);
		vmath::vec4* droplet = (vmath::vec4 *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 256 * sizeof(vmath::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for (int i = 0; i < 256; i++)
		{
			droplet[i][0] = droplet_x_offset[i];
			droplet[i][1] = 2.0f - fmodf((time + float(i)) * droplet_fall_speed[i], 4.31f);
			droplet[i][2] = time * droplet_rot_speed[i];
			droplet[i][3] = 0.0f;
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		int alien_index;
		for (alien_index = 0; alien_index < 256; alien_index++)
		{
			glVertexAttribI1i(0, alien_index);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

private:
	Shader shader;
	GLuint VAO;
	GLuint tex_alien_arr;
	GLuint rain_buffer;

	float droplet_x_offset[256];
	float droplet_rot_speed[256];
	float droplet_fall_speed[256];
};