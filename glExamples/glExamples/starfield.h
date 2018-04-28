#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

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

class starfield : public baseClass::application {
public:
	void start() {
		shader = Shader("shaders/starfield/starfield.vs", "shaders/starfield/starfield.fs");
		uniforms.time = glGetUniformLocation(shader.Program, "time");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");

		glGenTextures(1, &tex);
		tex = ktx::load("resource/brick.ktx");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(shader.Program, "tex_star"), 0);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		struct star_t
		{
			vmath::vec3     position;
			vmath::vec3     color;
		};

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 200 * sizeof(star_t), NULL, GL_STATIC_DRAW);

		star_t * star = (star_t *)glMapBufferRange(GL_ARRAY_BUFFER, 0, 200 * sizeof(star_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		int i;

		for (i = 0; i < 1000; i++)
		{
			star[i].position[0] = (random_float() * 2.0f - 1.0f) * 100.0f;
			star[i].position[1] = (random_float() * 2.0f - 1.0f) * 100.0f;
			star[i].position[2] = random_float();
			star[i].color[0] = 0.8f + random_float() * 0.2f;
			star[i].color[1] = 0.8f + random_float() * 0.2f;
			star[i].color[2] = 0.8f + random_float() * 0.2f;
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(star_t), (void *)sizeof(vmath::vec3));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat one[] = { 1.0f };
		float t = (float)time;
		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		t *= 0.1f;
		t -= floor(t);

		glViewport(0, 0, 800.0, 600.0);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, one);

		shader.Use();
		
		glUniform1f(uniforms.time, t);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glBindVertexArray(vao);
		
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, 200);
	}

private:
	Shader shader;
	GLuint vao;
	GLuint vbo;
	GLuint tex;

	struct
	{
		int         time;
		int         proj_matrix;
	} uniforms;
};