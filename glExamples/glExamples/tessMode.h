#pragma once
#include "BaseClass.h"

class TessMode : public baseClass::application {
public:
	void start() {
		quad_shader = Shader("shaders/tessMode/quad.vs", "shaders/tessMode/quad.fs", nullptr, "shaders/tessMode/quad.tcs", "shaders/tessMode/quad.tes");

		triangle_shader = Shader("shaders/tessMode/triangle.vs", "shaders/tessMode/triangle.fs", nullptr, "shaders/tessMode/triangle.tcs", "shaders/tessMode/triangle.tes");

		isolines_shader = Shader("shaders/tessMode/isoLines.vs", "shaders/tessMode/isoLines.fs", nullptr, "shaders/tessMode/isoLines.tcs", "shaders/tessMode/isoLines.tes");

		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f,0.0f,0.0f,1.0f };
		glClearBufferfv(GL_COLOR, 0, black);

		//quad_shader.Use();

		//triangle_shader.Use();

		isolines_shader.Use();

		glDrawArrays(GL_PATCHES, 0, 4);
	}

private:
	Shader quad_shader;

	Shader triangle_shader;

	Shader isolines_shader;
};