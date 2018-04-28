#include "Object.h"
#include "Tool.h"
#include <stdio.h>

namespace baseClass {
	object::object() :
		data_buffer(0),
		index_type(0),
		vao(0) {

	}

	object::~object() {

	}

	void object::load(const char* filename) {
		FILE* fp;
		fopen_s(&fp, filename, "rb");
		size_t fileSize;
		char* data;

		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		data = new char[fileSize];
		fread(data, fileSize, 1, fp);
		char* ptr = data;
		SB6M_HEADER* header = (SB6M_HEADER*)ptr;
		ptr += header->size;

		SB6M_CHUNK_SUB_OBJECT_LIST*  sub_object_chunk = nullptr;
		SB6M_VERTEX_ATTRIB_CHUNK* vertex_attrib_chunk = nullptr;
		SB6M_CHUNK_VERTEX_DATA* vertex_data_chunk = nullptr;
		SB6M_CHUNK_INDEX_DATA* index_data_chunk = nullptr;
		SB6M_DATA_CHUNK* data_chunk = nullptr;

		for (size_t i = 0; i < header->num_chunks; i++)
		{
			SB6M_CHUNK_HEADER * chunk = (SB6M_CHUNK_HEADER *)ptr;
			ptr += chunk->size;
			switch (chunk->chunk_type)
			{
			case SB6M_CHUNK_TYPE_VERTEX_ATTRIBS:
				vertex_attrib_chunk = (SB6M_VERTEX_ATTRIB_CHUNK *)chunk;
				break;
			case SB6M_CHUNK_TYPE_VERTEX_DATA:
				vertex_data_chunk = (SB6M_CHUNK_VERTEX_DATA *)chunk;
				break;
			case SB6M_CHUNK_TYPE_INDEX_DATA:
				index_data_chunk = (SB6M_CHUNK_INDEX_DATA *)chunk;
				break;
			case SB6M_CHUNK_TYPE_SUB_OBJECT_LIST:
				sub_object_chunk = (SB6M_CHUNK_SUB_OBJECT_LIST *)chunk;
				break;
			case SB6M_CHUNK_TYPE_DATA:
				data_chunk = (SB6M_DATA_CHUNK *)chunk;
				break;
			default:
				break; // goto failed;
			}
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		if (data_chunk != nullptr)
		{
			glGenBuffers(1, &data_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
			glBufferData(GL_ARRAY_BUFFER, data_chunk->data_length, (unsigned char*)data_chunk + data_chunk->data_offset, GL_STATIC_DRAW);
		}
		else
		{
			unsigned int data_size = 0;
			unsigned int size_used = 0;

			if (vertex_data_chunk != NULL)
			{
				data_size += vertex_data_chunk->data_size;
			}

			if (index_data_chunk != NULL)
			{
				data_size += index_data_chunk->index_count * (index_data_chunk->index_type == GL_UNSIGNED_SHORT ? sizeof(GLushort) : sizeof(GLubyte));
			}

			glGenBuffers(1, &data_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
			glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_STATIC_DRAW);

			if (vertex_data_chunk != NULL)
			{
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data_chunk->data_size, data + vertex_data_chunk->data_offset);
				size_used += vertex_data_chunk->data_offset;
			}

			if (index_data_chunk != NULL)
			{
				glBufferSubData(GL_ARRAY_BUFFER, size_used, index_data_chunk->index_count * (index_data_chunk->index_type == GL_UNSIGNED_SHORT ? sizeof(GLushort) : sizeof(GLubyte)), data + index_data_chunk->index_data_offset);
			}
		}

		for (unsigned int i = 0; i < vertex_attrib_chunk->attrib_count; i++)
		{
			SB6M_VERTEX_ATTRIB_DECL &attrib_decl = vertex_attrib_chunk->attrib_data[i];
			glVertexAttribPointer(i,
				attrib_decl.size,
				attrib_decl.type,
				attrib_decl.flags & SB6M_VERTEX_ATTRIB_FLAG_NORMALIZED ? GL_TRUE : GL_FALSE,
				attrib_decl.stride,
				(GLvoid *)(uintptr_t)attrib_decl.data_offset);
			glEnableVertexAttribArray(i);
		}

		if (index_data_chunk != NULL)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data_buffer);
			index_type = index_data_chunk->index_type;
			index_offset = index_data_chunk->index_data_offset;
		}
		else
		{
			index_type = GL_NONE;
		}

		if (sub_object_chunk != NULL)
		{
			if (sub_object_chunk->count > MAX_SUB_OBJECTS)
			{
				sub_object_chunk->count = MAX_SUB_OBJECTS;
			}

			for (unsigned int i = 0; i < sub_object_chunk->count; i++)
			{
				sub_object[i] = sub_object_chunk->sub_object[i];
			}

			num_sub_objects = sub_object_chunk->count;
		}
		else
		{
			sub_object[0].first = 0;
			sub_object[0].count = index_type != GL_NONE ? index_data_chunk->index_count : vertex_data_chunk->total_vertices;
			num_sub_objects = 1;
		}

		delete[] data;

		fclose(fp);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void object::free()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &data_buffer);

		vao = 0;
		data_buffer = 0;
	}

	void object::render_sub_object(unsigned int object_index, unsigned int instance_count, unsigned int base_instance)
	{
		glBindVertexArray(vao);

		if (index_type != GL_NONE)
		{
			glDrawElementsInstancedBaseInstance(GL_TRIANGLES,
				sub_object[object_index].count,
				index_type,
				(void*)sub_object[object_index].first,
				instance_count,
				base_instance);
		}
		else
		{
			glDrawArraysInstancedBaseInstance(GL_TRIANGLES,
				sub_object[object_index].first,
				sub_object[object_index].count,
				instance_count,
				base_instance);
		}
	}
}