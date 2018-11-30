#pragma once
#include "BaseClass.h"
#include <omp.h>

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

class OMP : public baseClass::application {
public:
	void start()
	{
		frame_index = 0;

		particles[0] = new PARTICLE[PARTICLE_COUNT];
		particles[1] = new PARTICLE[PARTICLE_COUNT];

		glGenBuffers(1, &particle_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particle_buffer);
		glBufferStorage(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(PARTICLE), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
		mapped_buffer = (PARTICLE*)glMapBufferRange(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(PARTICLE), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

		for (int i = 0; i < PARTICLE_COUNT; i++)
		{
			particles[0][i].position[0] = random_float() * 6.0f - 3.0f;
			particles[0][i].position[1] = random_float() * 6.0f - 3.0f;
			particles[0][i].position[2] = random_float() * 6.0f - 3.0f;
			particles[0][i].velocity = particles[0][i].position * 0.001f;

			mapped_buffer[i] = particles[0][i];
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
		glBindVertexBuffer(0, particle_buffer, 0, sizeof(PARTICLE));
		glEnableVertexAttribArray(0);

		shader = Shader("E:/vsProjects/glExamples/glExamples/shaders/ompparticles/render.vs", "E:/vsProjects/glExamples/glExamples/shaders/ompparticles/render.fs");
		int maxThreads = omp_get_max_threads();
		omp_set_num_threads(maxThreads);

	}

	void render(double time)
	{
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static double previousTime = 0.0;

		// Calculate delta time
		float deltaTime = (float)(time - previousTime);
		previousTime = time;

		// Update particle positions using OpenMP... or not.

		updatepaticles(deltaTime * 0.001f);


		// Clear
		glViewport(0, 0, 800, 600);
		glClearBufferfv(GL_COLOR, 0, black);

		// Bind our vertex arrays
		glBindVertexArray(vao);

		// Let OpenGL know we've changed the contents of the buffer
		glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(PARTICLE));

		glPointSize(3.0f);

		// Draw!
		shader.Use();
		glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);
	}

	void updatepaticles(double time)
	{
		const PARTICLE* const __restrict src = particles[frame_index & 1];
		PARTICLE* const __restrict dst = particles[(frame_index + 1) & 1];

		// For each particle in the system
#pragma omp parallel for schedule (dynamic, 16)
		for (int i = 0; i < PARTICLE_COUNT; i++)
		{
			// Get my own data
			const PARTICLE& me = src[i];
			vmath::vec3 delta_v(0.0f);

			// For all the other particles
			for (int j = 0; j < PARTICLE_COUNT; j++)
			{
				if (i != j) // ... not me!
				{
					//  Get the vector to the other particle
					vmath::vec3 delta_pos = src[j].position - me.position;
					float distance = vmath::length(delta_pos);
					// Normalize
					vmath::vec3 delta_dir = delta_pos / distance;
					// This clamp stops the system from blowing up if particles get
					// too close...
					distance = distance < 0.005f ? 0.005f : distance;
					// Update velocity
					delta_v += (delta_dir / (distance * distance));
				}
			}
			// Add my current velocity to my position.
			dst[i].position = me.position + me.velocity;
			// Produce new velocity from my current velocity plus the calculated delta
			dst[i].velocity = me.velocity + delta_v * time * 0.01f;
			// Write to mapped buffer
			mapped_buffer[i].position = dst[i].position;
		}

		// Count frames so we can double buffer next frame
		frame_index++;
	}

private:
	enum
	{
		PARTICLE_COUNT = 256
	};

	struct PARTICLE
	{
		vmath::vec3 position;
		vmath::vec3 velocity;
	};

	Shader shader;
	GLuint particle_buffer;
	PARTICLE* mapped_buffer;
	PARTICLE* particles[12];
	int frame_index;
	GLuint vao;
};