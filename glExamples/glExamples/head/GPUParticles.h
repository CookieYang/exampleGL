#pragma once

#include "BaseClass.h"
#include "KTXLoader.h"

static const int ParticlesWidth = 100;

class gpuParticles : public baseClass::application {
	void start() {
		GLfloat particlesData[ParticlesWidth * ParticlesWidth * 4];
		GLfloat particlesPosition[ParticlesWidth * ParticlesWidth * 2];
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
		for (size_t i = 0; i < ParticlesWidth; i++)
		{
			for (size_t j = 0; j < ParticlesWidth; j++)
			{
				particlesPosition[j * 2 + 0 + ParticlesWidth * i * 2] = (GLfloat)j / (GLfloat)ParticlesWidth;
				particlesPosition[j * 2 + 1 + ParticlesWidth * i * 2] = (GLfloat)i / (GLfloat)ParticlesWidth;
			}
		}

		sceneShader = Shader("D://ogl/glExamples/glExamples/shaders/scene.vs", "D://ogl/glExamples/glExamples/shaders/scene.fs");
		updateShader = Shader("D://ogl/glExamples/glExamples/shaders/update.vs", "D://ogl/glExamples/glExamples/shaders/update.fs");

		sceneUniform.mvpMatrix = glGetUniformLocation(sceneShader.Program, "mvp_Matrix");
		sceneUniform.particleDataTex = glGetUniformLocation(sceneShader.Program, "particlesDataTexture");
		sceneUniform.particleTex = glGetUniformLocation(sceneShader.Program, "particleTexture");
		sceneUniform.particlePosition = glGetAttribLocation(sceneShader.Program, "position");


		updateUniform.particleDataTex = glGetUniformLocation(updateShader.Program, "particleDataTex");
		updateUniform.Width = glGetUniformLocation(updateShader.Program, "Width");
		updateUniform.time = glGetUniformLocation(updateShader.Program, "time");
		updateUniform.particlePosition = glGetAttribLocation(updateShader.Program, "position");

		glGenTextures(1, &particleTexture);
		particleTexture = ktx::load("D://ogl/media/brick.ktx");
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenTextures(2, updateTextures);
		glBindTexture(GL_TEXTURE_2D, updateTextures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ParticlesWidth, ParticlesWidth, 0, GL_RGBA, GL_FLOAT, particlesData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, updateTextures[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ParticlesWidth, ParticlesWidth, 0, GL_RGBA, GL_FLOAT, particlesData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(2, updateFrameBuffers);
		glBindFramebuffer(GL_FRAMEBUFFER, updateFrameBuffers[0]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, particlesData[0], 0);

		glBindFramebuffer(GL_FRAMEBUFFER, updateFrameBuffers[1]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, particlesData[1], 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glGenBuffers(1, &particleVertexPositionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVertexPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, ParticlesWidth * ParticlesWidth * 2 * sizeof(GLfloat), (GLfloat*)particlesPosition, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		sceneShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(sceneUniform.particleTex, 0);
		glBindTexture(GL_TEXTURE_2D, particleTexture);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(sceneUniform.particleDataTex, 1);

		glGenVertexArrays(1, &sceneVAO);
		glBindVertexArray(sceneVAO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVertexPositionVBO);
		glVertexAttribPointer(sceneUniform.particlePosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(sceneUniform.particlePosition);

		updateShader.Use();
		glUniform1i(updateUniform.particleDataTex, 1);
		glUniform1i(updateUniform.Width, ParticlesWidth);

		glGenVertexArrays(1, &updateVAO);
		glBindVertexArray(updateVAO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVertexPositionVBO);
		glVertexAttribPointer(updateUniform.particlePosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(updateUniform.particlePosition);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	void render(double time) {

		glViewport(0, 0, 800, 600);
		
		vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 50.0f);

		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		vmath::mat4 model_matrix = vmath::scale(1.0f);

		vmath::mat4 project_matrix = vmath::perspective(50.0f,
			(float)800.0 / (float)600.0,
			0.1f,
			1000.0f);

		vmath::mat4 mvp_matrix = project_matrix * view_matrix * model_matrix;


	}

private:

	Shader sceneShader;
	Shader updateShader;

	GLuint particleTexture;

	GLuint updateFrameBuffers[2];
	GLuint updateTextures[2];


	GLuint particleVertexPositionVBO;
	GLuint sceneVAO;
	GLuint updateVAO;

	struct 
	{
		GLint particleTex;
		GLint particlePosition;
		GLint particleDataTex;
		GLint mvpMatrix;
	}sceneUniform;

	struct
	{
		GLint time;
		GLint particlePosition;
		GLint particleDataTex;
		GLint Width;
	}updateUniform;
};