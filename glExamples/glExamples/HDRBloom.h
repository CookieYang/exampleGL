#pragma once
#include "BaseClass.h"

enum
{
	MAX_SCENE_WIDTH = 2048,
	MAX_SCENE_HEIGHT = 2048,
	SPHERE_COUNT = 32,
};

class HDRBloom : public baseClass::application {
public:
	void start() {
		exposure = 1.0f;
		bloom_factor = 1.0f;
		bloom_thresh_min = 0.8f;
		bloom_thresh_max = 1.2f;

		renderShader = Shader("shaders/HDRBloom/scene.vs", "shaders/HDRBloom/scene.fs");
		uniforms.scene.bloom_thresh_min = glGetUniformLocation(renderShader.Program, "bloom_thresh_min");
		uniforms.scene.bloom_thresh_max = glGetUniformLocation(renderShader.Program, "bloom_thresh_max");
		filterShader = Shader("shaders/HDRBloom/filter.vs", "shaders/HDRBloom/filter.fs");
		resolveShader = Shader("shaders/HDRBloom/resolve.vs", "shaders/HDRBloom/resolve.fs");
		uniforms.resolve.exposure = glGetUniformLocation(resolveShader.Program, "exposure");
		uniforms.resolve.bloom_factor = glGetUniformLocation(resolveShader.Program, "bloom_factor");
		uniforms.resolve.scene_factor = glGetUniformLocation(resolveShader.Program, "scene_factor");

		int i;
		static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };

		glGenTextures(1, &tex_scene);
		glBindTexture(GL_TEXTURE_2D, tex_scene);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_scene, 0);
		glGenTextures(1, &tex_brightpass);
		glBindTexture(GL_TEXTURE_2D, tex_brightpass);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tex_brightpass, 0);
		glGenTextures(1, &tex_depth);
		glBindTexture(GL_TEXTURE_2D, tex_depth);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0);
		glDrawBuffers(2, buffers);

		glGenFramebuffers(2, &filter_fbo[0]);
		glGenTextures(2, &tex_filter[0]);
		for (i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[i]);
			glBindTexture(GL_TEXTURE_2D, tex_filter[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, i ? MAX_SCENE_WIDTH : MAX_SCENE_HEIGHT, i ? MAX_SCENE_HEIGHT : MAX_SCENE_WIDTH);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_filter[i], 0);
			glDrawBuffers(1, buffers);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenTextures(1, &tex_lut);
		glBindTexture(GL_TEXTURE_1D, tex_lut);
		glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		obj.load("resource/torus.sbm");

		glGenBuffers(1, &ubo_transform);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_transform);
		glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(vmath::mat4), NULL, GL_DYNAMIC_DRAW);

		struct material
		{
			vmath::vec3     diffuse_color;
			unsigned int : 32;           // pad
			vmath::vec3     specular_color;
			float           specular_power;
			vmath::vec3     ambient_color;
			unsigned int : 32;           // pad
		};

		glGenBuffers(1, &ubo_material);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_material);
		glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), NULL, GL_STATIC_DRAW);

		material * m = (material *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		float ambient = 0.002f;
		for (i = 0; i < SPHERE_COUNT; i++)
		{
			float fi = 3.14159267f * (float)i / 8.0f;
			m[i].diffuse_color = vmath::vec3(sinf(fi) * 0.5f + 0.5f, sinf(fi + 1.345f) * 0.5f + 0.5f, sinf(fi + 2.567f) * 0.5f + 0.5f);
			m[i].specular_color = vmath::vec3(2.8f, 2.8f, 2.9f);
			m[i].specular_power = 30.0f;
			m[i].ambient_color = vmath::vec3(ambient * 0.025f);
			ambient *= 1.5f;
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	void render(double time) {
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat one = 1.0f;
		int i;
		static double last_time = 0.0;
		static double total_time = 0.0;

	
		total_time += (time - last_time);
		last_time = time;
		float t = (float)total_time;

		glViewport(0, 0, 800.0f, 600.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_COLOR, 1, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		renderShader.Use();

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_transform);
		struct transforms_t
		{
			vmath::mat4 mat_proj;
			vmath::mat4 mat_view;
			vmath::mat4 mat_model[SPHERE_COUNT];
		} *transforms = (transforms_t *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(transforms_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		transforms->mat_proj = vmath::perspective(50.0f, (float)800.0f / (float)600.0f, 1.0f, 1000.0f);
		transforms->mat_view = vmath::translate(0.0f, 0.0f, -20.0f);
		for (i = 0; i < SPHERE_COUNT; i++)
		{
			float fi = 3.141592f * (float)i / 16.0f;
			// float r = cosf(fi * 0.25f) * 0.4f + 1.0f;
			float r = (i & 2) ? 0.6f : 1.5f;
			transforms->mat_model[i] = vmath::translate(cosf(t + fi) * 5.0f * r, sinf(t + fi * 4.0f) * 4.0f, sinf(t + fi) * 5.0f * r) *
				vmath::rotate(sinf(t + fi * 2.13f) * 75.0f, cosf(t + fi * 1.37f) * 92.0f, 0.0f);
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);

		glUniform1f(uniforms.scene.bloom_thresh_min, bloom_thresh_min);
		glUniform1f(uniforms.scene.bloom_thresh_max, bloom_thresh_max);

		obj.render(SPHERE_COUNT);

		glDisable(GL_DEPTH_TEST);

		filterShader.Use();

		glBindVertexArray(vao);

		glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[0]);
		glBindTexture(GL_TEXTURE_2D, tex_brightpass);
		glViewport(0, 0, 800.0f, 600.0f);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[1]);
		glBindTexture(GL_TEXTURE_2D, tex_filter[0]);
		glViewport(0, 0, 800.0f, 600.0f);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		resolveShader.Use();

		glUniform1f(uniforms.resolve.exposure, exposure);

		glUniform1f(uniforms.resolve.bloom_factor,  bloom_factor);
		glUniform1f(uniforms.resolve.scene_factor, 1.0f );


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_filter[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,  tex_scene);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

private:
	baseClass::object obj;

	Shader renderShader;
	Shader filterShader;
	Shader resolveShader;

	float exposure;
	float bloom_factor;
	float bloom_thresh_min;
	float bloom_thresh_max;

	GLuint      tex_scene;
	GLuint      tex_brightpass;
	GLuint      tex_depth;
	GLuint      tex_filter[2];
	GLuint		tex_lut;

	GLuint ubo_transform;
	GLuint ubo_material;

	GLuint vao;
	GLuint render_fbo;
	GLuint filter_fbo[2];

	struct
	{
		struct
		{
			int bloom_thresh_min;
			int bloom_thresh_max;
		} scene;
		struct
		{
			int exposure;
			int bloom_factor;
			int scene_factor;
		} resolve;
	} uniforms;
};