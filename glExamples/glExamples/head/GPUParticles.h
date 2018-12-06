#pragma once

#include "BaseClass.h"

static const int ParticlesWidth = 100;

class gpuParticles : public baseClass::application {
	void start() {
		GLfloat particlesData[ParticlesWidth * ParticlesWidth * 4];
		for (size_t i = 0; i < ParticlesWidth; i++)
		{
			for (size_t j = 0; j < ParticlesWidth; j++)
			{
				particlesData[j * 4 + 0 + ParticlesWidth * i * 4] = 0.0f;
				particlesData[j * 4 + 1 + ParticlesWidth * i * 4] = 0.0f;
				particlesData[j * 4 + 2 + ParticlesWidth * i * 4] = 0.0f;
				particlesData[j * 4 + 3 + ParticlesWidth * i * 4] = -1.0f;
			}
		}
	}

	void render(double time) {

	}

private:

	Shader sceneShader;
	Shader updateShader;

	GLuint particleTexture;

	GLuint updateFrameBuffers[2];
	GLuint updateTextures[2];
	GLuint particleVertexPositionVBO;
};