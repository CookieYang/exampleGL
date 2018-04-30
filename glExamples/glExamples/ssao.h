#pragma once
#include "BaseClass.h"

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

class SSAO : public baseClass::application {
public:
	void start()
	{
		renderShader = Shader("shaders/ssao/render.vs", "shaders/ssao/render.fs");
		uniforms.render.mv_matrix = glGetUniformLocation(renderShader.Program, "mv_matrix");
		uniforms.render.proj_matrix = glGetUniformLocation(renderShader.Program, "proj_matrix");
		uniforms.render.shading_level = glGetUniformLocation(renderShader.Program, "shading_level");
		ssaoShader = Shader("shaders/ssao/ssao.vs", "shaders/ssao/ssao.fs");
		uniforms.ssao.ssao_radius = glGetUniformLocation(ssaoShader.Program, "ssao_radius");
		uniforms.ssao.ssao_level = glGetUniformLocation(ssaoShader.Program, "ssao_level");
		uniforms.ssao.object_level = glGetUniformLocation(ssaoShader.Program, "object_level");
		uniforms.ssao.weight_by_angle = glGetUniformLocation(ssaoShader.Program, "weight_by_angle");
		uniforms.ssao.randomize_points = glGetUniformLocation(ssaoShader.Program, "randomize_points");
		uniforms.ssao.point_count = glGetUniformLocation(ssaoShader.Program, "point_count");

		glGenFramebuffers(1, &render_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
		glGenTextures(3, fbo_textures);

		glBindTexture(GL_TEXTURE_2D, fbo_textures[0]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, 2048, 2048);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, fbo_textures[1]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 2048, 2048);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, fbo_textures[2]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo_textures[0], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, fbo_textures[1], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo_textures[2], 0);

		static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers(2, draw_buffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(quad_vao);

		obj.load("media/dragon.sbm");
		cube.load("media/cube.sbm");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		int i;
		SAMPLE_POINTS point_data;

		for (i = 0; i < 256; i++)
		{
			//do
		//	{
				point_data.point[i][0] = random_float() * 2.0f - 1.0f;
				point_data.point[i][1] = random_float() * 2.0f - 1.0f;
				point_data.point[i][2] = random_float(); //  * 2.0f - 1.0f;
				point_data.point[i][3] = 0.0f;
			//} while (length(point_data.point[i]) > 1.0f);
			//normalize(point_data.point[i]);
		}
		for (i = 0; i < 256; i++)
		{
			point_data.random_vectors[i][0] = random_float();
			point_data.random_vectors[i][1] = random_float();
			point_data.random_vectors[i][2] = random_float();
			point_data.random_vectors[i][3] = random_float();
		}

		glGenBuffers(1, &points_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, points_buffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &point_data, GL_STATIC_DRAW);
	}

	void render(double time)
	{
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		static const GLfloat one = 1.0f;
		static double last_time = 0.0;
		static double total_time = 0.0;
		static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	
		total_time += (time - last_time);
		last_time = time;

		float f = (float)total_time;

		glViewport(0, 0, 800, 600);

		glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
		glEnable(GL_DEPTH_TEST);

		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_COLOR, 1, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, points_buffer);

		renderShader.Use();

		const vmath::mat4 lookat_matrix = vmath::lookat(vmath::vec3(0.0f, 3.0f, 15.0f),
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			800.0f / 600.0f,
			0.1f,
			1000.0f);
		glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, proj_matrix);

		vmath::mat4 mv_matrix = vmath::translate(0.0f, -5.0f, 0.0f) *
			vmath::rotate(f * 5.0f, 0.0f, 1.0f, 0.0f) *
			vmath::mat4::identity();
		glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, lookat_matrix * mv_matrix);

		glUniform1f(uniforms.render.shading_level, 0.7f);

		obj.render();

		mv_matrix = vmath::translate(0.0f, -4.5f, 0.0f) *
			vmath::rotate(f * 5.0f, 0.0f, 1.0f, 0.0f) *
			vmath::scale(4000.0f, 0.1f, 4000.0f) *
			vmath::mat4::identity();
		glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, lookat_matrix * mv_matrix);

		cube.render();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ssaoShader.Use();

		glUniform1f(uniforms.ssao.ssao_radius, 0.05f *800.0f / 1000.0f);
		glUniform1f(uniforms.ssao.ssao_level, 0.3f);
		// glUniform1i(uniforms.ssao.weight_by_angle, weight_by_angle ? 1 : 0);
		glUniform1i(uniforms.ssao.randomize_points, 1);
		glUniform1ui(uniforms.ssao.point_count, 10);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo_textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fbo_textures[1]);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
private:
	Shader renderShader;
	Shader ssaoShader;

	GLuint render_fbo;
	GLuint fbo_textures[3];
	GLuint quad_vao;
	GLuint points_buffer;

	baseClass::object obj;
	baseClass::object cube;

	struct
	{
		struct
		{
			GLint           mv_matrix;
			GLint           proj_matrix;
			GLint           shading_level;
		} render;
		struct
		{
			GLint           ssao_level;
			GLint           object_level;
			GLint           ssao_radius;
			GLint           weight_by_angle;
			GLint           randomize_points;
			GLint           point_count;
		} ssao;
	} uniforms;

	struct SAMPLE_POINTS
	{
		vmath::vec4     point[256];
		vmath::vec4     random_vectors[256];
	};
};