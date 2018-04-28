#pragma once
#include "BaseClass.h"

#define FBO_SIZE                2048
#define FRUSTUM_DEPTH           1000

class dof : public baseClass::application {
public:
	void start() {
		focal_distance = 40.0f;
		focal_depth = 50.0f;

		view_program = Shader("shaders/dof/render.vs", "shaders/dof/render.fs");
		uniforms.view.proj_matrix = glGetUniformLocation(view_program.Program, "proj_matrix");
		uniforms.view.mv_matrix = glGetUniformLocation(view_program.Program, "mv_matrix");
		uniforms.view.full_shading = glGetUniformLocation(view_program.Program, "full_shading");
		uniforms.view.diffuse_albedo = glGetUniformLocation(view_program.Program, "diffuse_albedo");
		display_program = Shader("shaders/dof/display.vs", "shaders/dof/display.fs");
		uniforms.dof.focal_distance = glGetUniformLocation(display_program.Program, "focal_distance");
		uniforms.dof.focal_depth = glGetUniformLocation(display_program.Program, "focal_depth");
		filter_program = Shader::ComputeProgram("shaders/dof/compute.cs");

		static const char * const object_names[] =
		{
			"media/dragon.sbm",
			"media/sphere.sbm",
			"media/cube.sbm",
			"media/cube.sbm",
			"media/cube.sbm",
		};

		static const vmath::vec4 object_colors[] =
		{
			vmath::vec4(1.0f, 0.7f, 0.8f, 1.0f),
			vmath::vec4(0.7f, 0.8f, 1.0f, 1.0f),
			vmath::vec4(0.3f, 0.9f, 0.4f, 1.0f),
			vmath::vec4(0.6f, 0.4f, 0.9f, 1.0f),
			vmath::vec4(0.8f, 0.2f, 0.1f, 1.0f),
		};

		for (size_t i = 0; i < 5; i++)
		{
			objects[i].obj.load(object_names[i]);
			objects[i].diffuse_albedo = object_colors[i];
		}

		glGenFramebuffers(1, &depth_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

		glGenTextures(1, &depth_tex);
		glBindTexture(GL_TEXTURE_2D, depth_tex);
		glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, FBO_SIZE, FBO_SIZE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenTextures(1, &temp_tex);
		glBindTexture(GL_TEXTURE_2D, temp_tex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

		glGenTextures(1, &color_tex);
		glBindTexture(GL_TEXTURE_2D, color_tex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_tex, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_tex, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);

		glGenVertexArrays(1, &quad_vao);
		glBindVertexArray(quad_vao);
	}

	void render(double time) {
		
		static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		static double last_time = 0.0;
		static double total_time = 0.0;

		total_time += (time - last_time);
		last_time = time;

		const float f = (float)total_time + 30.0f;

		vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 40.0f);

		camera_proj_matrix = vmath::perspective(50.0f,
			(float)800.0f / (float)600.0f,
			2.0f,
			300.0f);

		camera_view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		objects[0].model_matrix = vmath::translate(5.0f, 0.0f, 20.0f) *
			vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		objects[1].model_matrix = vmath::translate(-5.0f, 0.0f, 0.0f) *
			vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		objects[2].model_matrix = vmath::translate(-15.0f, 0.0f, -20.0f) *
			vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		objects[3].model_matrix = vmath::translate(-25.0f, 0.0f, -40.0f) *
			vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		objects[4].model_matrix = vmath::translate(-35.0f, 0.0f, -60.0f) *
			vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		glEnable(GL_DEPTH_TEST);
		render_scene(total_time);

		glUseProgram(filter_program);

		glBindImageTexture(0, color_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, temp_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDispatchCompute(600.0f, 1, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glBindImageTexture(0, temp_tex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, color_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDispatchCompute(800.0f, 1, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, color_tex);
		glDisable(GL_DEPTH_TEST);
		display_program.Use();
		glUniform1f(uniforms.dof.focal_distance, focal_distance);
		glUniform1f(uniforms.dof.focal_depth, focal_depth);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	}

	// 把场景渲染到depth_fbo中
	void render_scene(double time) {
		static const GLfloat ones[] = { 1.0f };
		static const GLfloat zero[] = { 0.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
		static const GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
		static const vmath::mat4 scale_bias_matrix = vmath::mat4(vmath::vec4(0.5f, 0.0f, 0.0f, 0.0f),
			vmath::vec4(0.0f, 0.5f, 0.0f, 0.0f),
			vmath::vec4(0.0f, 0.0f, 0.5f, 0.0f),
			vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

		glDrawBuffers(1, attachments);
		glViewport(0, 0, 800.0f, 600.0f);
		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		view_program.Use();
		glUniformMatrix4fv(uniforms.view.proj_matrix, 1, GL_FALSE, camera_proj_matrix);

		glClearBufferfv(GL_DEPTH, 0, ones);

		int i;
		for (i = 0; i < OBJECT_COUNT; i++)
		{
			vmath::mat4& model_matrix = objects[i].model_matrix;
			glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * objects[i].model_matrix);
			glUniform3fv(uniforms.view.diffuse_albedo, 1, objects[i].diffuse_albedo);
			objects[0].obj.render();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

private:
	Shader          view_program;
	GLuint          filter_program;
	Shader          display_program;

	struct
	{
		struct
		{
			GLint   focal_distance;
			GLint   focal_depth;
		} dof;
		struct
		{
			GLint   mv_matrix;
			GLint   proj_matrix;
			GLint   full_shading;
			GLint   diffuse_albedo;
		} view;
	} uniforms;

	GLuint          depth_fbo;
	GLuint          depth_tex;
	GLuint          color_tex;
	GLuint          temp_tex;

	enum { OBJECT_COUNT = 5 };
	struct
	{
		baseClass::object obj;
		vmath::mat4     model_matrix;
		vmath::vec4     diffuse_albedo;
	} objects[OBJECT_COUNT];

	vmath::mat4     camera_view_matrix;
	vmath::mat4     camera_proj_matrix;

	float          focal_distance;
	float          focal_depth;

	GLuint          quad_vao;
};