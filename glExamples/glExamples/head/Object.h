#pragma once
#include "SB6MFile.h"

#ifndef SB6M_FILETYPES_ONLY

namespace baseClass{

	class object {

	public:

		object() ;
		~object() ;

		inline void render(unsigned int instance_count = 1, unsigned int base_instance = 0) {
			render_sub_object(0, instance_count, base_instance);
		}

		void render_sub_object(unsigned int object_index, unsigned int instance_count = 1, unsigned int base_instance = 0);

		void get_sub_object_info(unsigned int index, unsigned int &first, unsigned int &count) {
			if (index > num_sub_objects)
			{
				first = 0;
				count = 0;
			}
			else {
				first = sub_object[index].first;
				count = sub_object[index].count;
			}
		}

		unsigned int get_sub_object_count() const { return num_sub_objects; }
		unsigned int       get_vao() const { return vao; }
		void load(const char * filename);
		void free();

	private:
		unsigned int                  data_buffer;
		unsigned int                  vao;
		unsigned int                  index_type;
		unsigned int                  index_offset;

		enum { MAX_SUB_OBJECTS = 256 };

		unsigned int            num_sub_objects;
		SB6M_SUB_OBJECT_DECL    sub_object[MAX_SUB_OBJECTS];
	};
}
#endif