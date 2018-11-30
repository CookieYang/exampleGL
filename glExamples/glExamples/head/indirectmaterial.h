#pragma once
#include  "BaseClass.h"

struct DrawArraysIndirectCommand
{
	GLuint  count;
	GLuint  primCount;
	GLuint  first;
	GLuint  baseInstance;
};

struct MaterialProperties
{
	vmath::vec4     ambient;
	vmath::vec4     diffuse;
	vmath::vec3     specular;
	float           specular_power;
};

struct FrameUniforms
{
	vmath::mat4 view_matrix;
	vmath::mat4 proj_matrix;
	vmath::mat4 viewproj_matrix;
};

class IndirectMaterial : public baseClass::application
{
public:
	void start()
	{
		draws_per_frame = NUM_DRAWS;
		frame_count = 0;

		shader = Shader("E:/vsProjects/glExamples/glExamples/shaders/indirectmaterial/render.vs", "E:/vsProjects/glExamples/glExamples/shaders/indirectmaterial/render.fs");

		obj.load("media/asteroids.sbm");

		glGenBuffers(1, &indirect_draw_buffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_draw_buffer);
		glBufferStorage(GL_DRAW_INDIRECT_BUFFER, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), nullptr, GL_MAP_WRITE_BIT);
		DrawArraysIndirectCommand* cmd = (DrawArraysIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, NUM_DRAWS * sizeof(DrawArraysIndirectCommand), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		
		int num_objects = obj.get_sub_object_count();
		for (int i = 0; i < NUM_DRAWS; i++)
		{
			obj.get_sub_object_info(i % num_objects, cmd[i].first, cmd[i].count);
			cmd[i].primCount = 1;
			cmd[i].baseInstance = i % NUM_MATERIALS;
		}

		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

		glGenBuffers(1, &transform_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, transform_buffer);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_DRAWS * sizeof(vmath::mat4), nullptr, GL_MAP_WRITE_BIT);

		glGenBuffers(1, &frame_uniforms_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, frame_uniforms_buffer);
		glBufferStorage(GL_UNIFORM_BUFFER, sizeof(FrameUniforms), nullptr, GL_MAP_WRITE_BIT);

		glGenBuffers(1, &material_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, material_buffer);
		glBufferStorage(GL_UNIFORM_BUFFER, NUM_MATERIALS * sizeof(MaterialProperties), nullptr, GL_MAP_WRITE_BIT);

		MaterialProperties* pMaterial = (MaterialProperties*)
			glMapBufferRange(GL_UNIFORM_BUFFER,
				0,
				NUM_MATERIALS * sizeof(MaterialProperties),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for ( int i = 0; i < NUM_MATERIALS; i++)
		{
			float f = float(i) / float(NUM_MATERIALS);
			pMaterial[i].ambient = (vmath::vec4(sinf(f * 3.7f), sinf(f * 5.7f + 3.0f), sinf(f * 4.3f + 2.0f), 1.0f) + vmath::vec4(1.0f, 1.0f, 2.0f, 0.0f)) * 0.1f;
			pMaterial[i].diffuse = (vmath::vec4(sinf(f * 9.9f + 6.0f), sinf(f * 3.1f + 2.5f), sinf(f * 7.2f + 9.0f), 1.0f) + vmath::vec4(1.0f, 2.0f, 2.0f, 0.0f)) * 0.4f;
			pMaterial[i].specular = (vmath::vec3(sinf(f * 1.6f + 4.0f), sinf(f * 0.8f + 2.7f), sinf(f * 5.2f + 8.0f)) + vmath::vec3(19.0f, 19.0f, 19.0f)) * 0.6f;
			pMaterial[i].specular_power = 200.0f + sinf(f) * 50.0f;
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		glBindVertexArray(obj.get_vao());

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);

	}

	void render(double time)
	{
		float t = float(time);
		int i;

		static const float black[] = { 0.0, 0.0, 0.0, 1.0 };
		static const float white[] = { 1.0, 1.0, 1.0, 1.0 };

		const vmath::mat4 view_matrix = vmath::lookat(vmath::vec3(30.0f * cosf(t * 0.023f), 30.0f * cosf(t * 0.023f), 30.0f * sinf(t * 0.037f) - 200.0f),
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::normalize(vmath::vec3(0.1f - cosf(t * 0.1f) * 0.3f, 1.0f, 0.0f)));
		const vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			800.0 / 600.0,
			1.0f,
			2000.0f);

		glViewport(0, 0, 800, 600);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, white);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, frame_uniforms_buffer);
		FrameUniforms* pUniforms = (FrameUniforms*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(FrameUniforms), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		pUniforms->proj_matrix = proj_matrix;
		pUniforms->view_matrix = view_matrix;
		pUniforms->viewproj_matrix = view_matrix * proj_matrix;
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		vmath::mat4* pModelMatrices = (vmath::mat4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, NUM_DRAWS * sizeof(vmath::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transform_buffer);

		float f = t * 0.1f;
		for (i = 0; i < draws_per_frame; i++)
		{
			vmath::mat4 m = vmath::translate(sinf(f * 7.3f) * 70.0f, sinf(f * 3.7f + 2.0f) * 70.0f, sinf(f * 2.9f + 8.0f) * 70.0f) *
				vmath::rotate(f * 330.0f, f * 490.0f, f * 250.0f);
			pModelMatrices[i] = m;
			f += 3.1f;
		}

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindBufferBase(GL_UNIFORM_BUFFER, 2, material_buffer);

		shader.Use();
		glBindVertexArray(obj.get_vao());
		glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, draws_per_frame, 0);

		frame_count++;
	}

private:
	Shader shader;
	baseClass::object obj;
	GLuint frame_uniforms_buffer;
	GLuint transform_buffer;
	GLuint indirect_draw_buffer;
	GLuint material_buffer;

	struct 
	{
		GLint time;
		GLint view_matrix;
		GLint proj_matrix;
		GLint viewproj_matrix;
	}uniforms;

	enum 
	{
		NUM_MATERIALS = 100,
		NUM_DRAWS       = 16384
	};

	unsigned int frame_count;
	unsigned int draws_per_frame;
};