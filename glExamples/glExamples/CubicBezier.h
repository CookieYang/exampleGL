#pragma once
#include "BaseClass.h"

class CubicBezier : public baseClass::application {
	void start() {
		shader = Shader("shaders/CubicBezier/Bezier.vs", "shaders/CubicBezier/Bezier.fs", nullptr, "shaders/CubicBezier/Bezier.tcs", "shaders/CubicBezier/Bezier.tes");

		uniforms.patch.mv_matrix = glGetUniformLocation(shader.Program, "mv_matrix");
		uniforms.patch.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
		uniforms.patch.mvp = glGetUniformLocation(shader.Program, "mvp");

		glGenVertexArrays(1, &patch_vao);
		glBindVertexArray(patch_vao);

		glGenBuffers(1, &patch_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, patch_buffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(patch_data), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		static const GLushort indices[] =
		{
			0, 1, 1, 2, 2, 3,
			4, 5, 5, 6, 6, 7,
			8, 9, 9, 10, 10, 11,
			12, 13, 13, 14, 14, 15,

			0, 4, 4, 8, 8, 12,
			1, 5, 5, 9, 9, 13,
			2, 6, 6, 10, 10, 14,
			3, 7, 7, 11, 11, 15
		};

		glGenBuffers(1, &cage_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cage_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	void render(double time) {
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat one = 1.0f;

		int i;
		static double last_time = 0.0;
		static double total_time = 0.0;

	
		total_time += (time - last_time);
		last_time = time;

		float t = (float)total_time;

		static const float patch_initializer[] =
		{
			-1.0f,  -1.0f,  0.0f,
			-0.33f, -1.0f,  0.0f,
			0.33f, -1.0f,  0.0f,
			1.0f,  -1.0f,  0.0f,

			-1.0f,  -0.33f, 0.0f,
			-0.33f, -0.33f, 0.0f,
			0.33f, -0.33f, 0.0f,
			1.0f,  -0.33f, 0.0f,

			-1.0f,   0.33f, 0.0f,
			-0.33f,  0.33f, 0.0f,
			0.33f,  0.33f, 0.0f,
			1.0f,   0.33f, 0.0f,

			-1.0f,   1.0f,  0.0f,
			-0.33f,  1.0f,  0.0f,
			0.33f,  1.0f,  0.0f,
			1.0f,   1.0f,  0.0f,
		};

		glViewport(0, 0, 800.0,600.0);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glEnable(GL_DEPTH_TEST);

		vmath::vec3 * p = (vmath::vec3 *)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(patch_data), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(p, patch_initializer, sizeof(patch_initializer));

		for (i = 0; i < 16; i++)
		{
			float fi = (float)i / 16.0f;
			p[i][2] = sinf(t * (0.2f + fi * 0.3f));
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		glBindVertexArray(patch_vao);

		shader.Use();

		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			1.0f, 1000.0f);
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::rotate(t * 10.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(t * 17.0f, 1.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(uniforms.patch.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(uniforms.patch.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(uniforms.patch.mvp, 1, GL_FALSE, proj_matrix * mv_matrix);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, 16);
	}

private:
	Shader shader;

	GLuint      patch_vao;
	GLuint      patch_buffer;
	GLuint      cage_indices;
	vmath::vec3 patch_data[16];

	struct
	{
		struct
		{
			int     mv_matrix;
			int     proj_matrix;
			int     mvp;
		} patch;
		struct
		{
			int     draw_color;
			int     mvp;
		} control_point;
	} uniforms;
};