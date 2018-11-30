#pragma once
#include "BaseClass.h"

enum
{
	NUM_DRAWS = 50000
};

struct DrawArraysIndirectCommand
{
	GLuint  count;
	GLuint  primCount;
	GLuint  first;
	GLuint  baseInstance;
};

class Asteroids : public baseClass::application {
public:
	void start() {
		load_shader();
		object.load("resource/asteroids.sbm");
		glGenBuffers(1, &indirect_draw_buffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), NULL, GL_STATIC_DRAW);
		DrawArraysIndirectCommand* cmds = (DrawArraysIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (size_t i = 0; i < NUM_DRAWS; i++)
		{
			object.get_sub_object_info(i % object.get_sub_object_count(), cmds[i].first, cmds[i].count);
			cmds[i].primCount = 1;
			cmds[i].baseInstance = i;
		}
		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

		glBindVertexArray(object.get_vao());
		glGenBuffers(1, &draw_index_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, draw_index_buffer);
		glBufferData(GL_ARRAY_BUFFER, NUM_DRAWS * sizeof(GLuint), NULL, GL_STATIC_DRAW);
		GLuint* draw_index = (GLuint*)glMapBufferRange(GL_ARRAY_BUFFER, 0, NUM_DRAWS * sizeof(GLuint), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (size_t i = 0; i < NUM_DRAWS; i++)
		{
			draw_index[i] = i;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glVertexAttribIPointer(10, 1, GL_UNSIGNED_INT, 0, NULL);
		glVertexAttribDivisor(10, 1);
		glEnableVertexAttribArray(10);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
	}

	void render(double time) {
		int j = 0;
		static const float one = 1.0f;
		static const GLfloat black[] = {0.0f,0.0f,0.0f,0.0f};
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);
		static double last_time = 0.0;
		static double total_time = 0.0;

		if (!paused)
		{
			total_time += time - last_time;
		}
		last_time = time;

		float t = float(total_time);
		int i = int(total_time * 3.0f);
		glViewport(0, 0, 800.0f, 600.0f);


		const vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(100.0f * cosf(t * 0.023f), 100.0f * cosf(t * 0.023f), 300.0f * sinf(t * 0.037f) - 600.0f),
			vmath::vec3(0.0f, 0.0f, 260.0f),
			vmath::normalize(vmath::vec3(0.1f - cosf(t * 0.1f) * 0.3f, 1.0f, 0.0f)));
		const vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			1.0f,
			2000.0f);

		shader.Use();

		glUniform1f(uniforms.time, t);
		glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(uniforms.viewproj_matrix, 1, GL_FALSE, proj_matrix * view_matrix);

		glBindVertexArray(object.get_vao());

		if (mode == MODE_MULTIDRAW)
		{
			glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, NUM_DRAWS, 0);
		}
		else if (mode == MODE_SEPARATE_DRAWS)
		{
			for (j = 0; j < NUM_DRAWS; j++)
			{
				GLuint first, count;
				object.get_sub_object_info(j % object.get_sub_object_count(), first, count);
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES,
					first,
					count,
					1, j);
			}
		}

	}

	void processInput(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			paused != paused;
		}
		else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		{
			vsync = !vsync;
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			mode = MODE(mode + 1);
			if (mode == MODE_MAX)
			{
				mode = MODE_FIRST;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			load_shader();
			
		}
	}

	void load_shader() {
		shader = Shader("shaders/asteroids.vs", "shaders/asteroids.fs");
		uniforms.time = glGetUniformLocation(shader.Program, "time");
		uniforms.view_matrix = glGetUniformLocation(shader.Program, "view_matrix");
		uniforms.proj_matrix = glGetUniformLocation(shader.Program, "proj_matrix");
		uniforms.viewproj_matrix = glGetUniformLocation(shader.Program, "viewproj_matrix");
	}

private:
	Shader shader;
	baseClass::object object;
	GLuint indirect_draw_buffer;
	GLuint draw_index_buffer;

	struct
	{
		GLint           time;
		GLint           view_matrix;
		GLint           proj_matrix;
		GLint           viewproj_matrix;
	} uniforms;

	enum MODE
	{
		MODE_FIRST,
		MODE_MULTIDRAW = 0,
		MODE_SEPARATE_DRAWS,
		MODE_MAX = MODE_SEPARATE_DRAWS
	};

	MODE                mode;
	bool                paused;
	bool                vsync;
};