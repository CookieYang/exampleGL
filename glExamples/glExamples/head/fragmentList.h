#pragma once
#include "KTXLoader.h"
#include "BaseClass.h"
class fragmentList : public baseClass::application {
public:

	void load_shaders() {
		clear_program = Shader("shaders/clear.vs", "shaders/clear.fs");
		append_program = Shader("shaders/append.vs", "shaders/append.fs");
		resolve_program = Shader("shaders/resolve.vs", "shaders/resolve.fs");
	}

	void processInput(GLFWwindow* window) {
		__super::processInput(window);
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			load_shaders();
		}
	}

	void start() {
		load_shaders();

		glGenBuffers(1, &uniforms_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

		object.load("resource/dragon.sbm");

		glGenBuffers(1, &fragment_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, fragment_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 1024 * 1024 * 16, NULL, GL_DYNAMIC_COPY);

		glGenBuffers(1, &atomic_counter_buffer);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter_buffer);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, 4, NULL, GL_DYNAMIC_COPY);

		glGenTextures(1, &head_pointer_image);
		glBindTexture(GL_TEXTURE_2D, head_pointer_image);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 1024, 1024);

		glGenVertexArrays(1, &dummy_vao);
		glBindVertexArray(dummy_vao);
	}

	void render(double currentTime)
	{
		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLfloat ones[] = { 1.0f };
		const float f = (float)currentTime;

		glViewport(0, 0, 800, 600);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		clear_program.Use();
		glBindVertexArray(dummy_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		append_program.Use();

		vmath::mat4 model_matrix = vmath::scale(7.0f);
		vmath::vec3 view_position = vmath::vec3(cosf(f * 0.35f) * 120.0f, cosf(f * 0.4f) * 30.0f, sinf(f * 0.35f) * 120.0f);
		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 30.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		vmath::mat4 mv_matrix = view_matrix * model_matrix;
		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, proj_matrix * mv_matrix);

		static const unsigned int zero = 0;
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_counter_buffer);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(zero), &zero);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fragment_buffer);

		glBindImageTexture(0, head_pointer_image, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		object.render();

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		resolve_program.Use();

		glBindVertexArray(dummy_vao);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

private:
	Shader clear_program;
	Shader append_program;
	Shader resolve_program;

	struct
	{
		GLuint      color;
		GLuint      normals;
	} textures;

	struct uniforms_block
	{
		vmath::mat4     mv_matrix;
		vmath::mat4     view_matrix;
		vmath::mat4     proj_matrix;
	};

	GLuint          uniforms_buffer;

	struct
	{
		GLint           mvp;
	} uniforms;

	baseClass::object     object;

	GLuint          fragment_buffer;
	GLuint          head_pointer_image;
	GLuint          atomic_counter_buffer;
	GLuint          dummy_vao;
};