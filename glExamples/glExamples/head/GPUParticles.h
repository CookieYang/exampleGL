#pragma once

#include "BaseClass.h"

class gpuParticles : public baseClass::application {
	void start() {

	}

	void render(double time) {

	}

private:

	Shader sceneShader;
	Shader updateShader;

	GLuint particleTexture;
};