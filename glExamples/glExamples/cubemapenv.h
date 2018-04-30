#pragma once
#include "BaseClass.h"
#include "KTXLoader.h"

class CubeMap : public baseClass::application {
public:
	void start() {
		envmap_index = 0;
		envShader = Shader("shaders/cubemap/skybox.vs", "shaders/cubemap/skybox.fs");
		uniforms.skybox.view_matrix = glGetUniformLocation(envShader.Program, "view_matrix");
		objShader = Shader("shaders/cubemap/render.vs", "shaders/cubemap/render.fs");
		uniforms.render.mv_matrix = glGetUniformLocation(objShader.Program, "mv_matrix");
		uniforms.render.proj_matrix = glGetUniformLocation(objShader.Program, "proj_matrix");
		envmaps[0] = ktx::load("media/envmaps/mountaincube.ktx");
		tex_envmap = envmaps[envmap_index];
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		obj.load("media/dragon.sbm");
		glGenVertexArrays(1, &skybox_vao);
		glBindVertexArray(skybox_vao);
		glDepthFunc(GL_LEQUAL);
	}
	void render(double time) {
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
		static const GLfloat ones[] = { 1.0f };
		const float t = (float)time * 0.1f;

		vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)800.0f / (float)600.0f, 0.1f, 1000.0f);
		vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t)),
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));
		vmath::mat4 mv_matrix = view_matrix *
			vmath::rotate(t, 1.0f, 0.0f, 0.0f) *
			vmath::rotate(t * 130.1f, 0.0f, 1.0f, 0.0f) *
			vmath::translate(0.0f, -4.0f, 0.0f);

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);

		glViewport(0, 0, 800, 600);

		envShader.Use();
		glBindVertexArray(skybox_vao);

		glUniformMatrix4fv(uniforms.skybox.view_matrix, 1, GL_FALSE, view_matrix);

		glDisable(GL_DEPTH_TEST);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		objShader.Use();

		glUniformMatrix4fv(uniforms.render.mv_matrix, 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(uniforms.render.proj_matrix, 1, GL_FALSE, proj_matrix);

		glEnable(GL_DEPTH_TEST);

		obj.render();
	}
private:
	Shader envShader;
	Shader objShader;
	GLuint tex_envmap;
	GLuint envmaps[3];
	GLuint skybox_vao;
	int envmap_index;
	struct 
	{
		struct
		{
			GLint mv_matrix;
			GLint proj_matrix;
		}render;
		struct 
		{
			GLint view_matrix;
		}skybox;
	}uniforms;
	baseClass::object obj;
};