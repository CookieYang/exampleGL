#version 430 core
layout (triangles) in ;
layout (triangle_strip, max_vertices = 6) out;

in mat4 g_view_matrix[3];
in vec3 g_normal[3];

uniform mat4 proj_matrix;
uniform vec4 light_position;

out vec3 f_normal;
out vec3 f_eye;
out vec4 f_color;
out vec3 f_light_direction;

void main(void){
	int i;
    mat4 translate_matrix = mat4(1.0f);
    vec4 position;
    translate_matrix[3][0] = -1.5f;

    for (i=0; i < gl_in.length(); i++)
    {
        position = g_view_matrix[i]*translate_matrix*gl_in[i].gl_Position;

		gl_Position = proj_matrix*position;

		// Normal is already in camera space
		f_normal = g_normal[i];
		
		f_eye = -vec3(position);
		
		f_color = vec4(1.0, 0.0, 0.0, 1.0);

		// Light position is in camera space
		f_light_direction = vec3(light_position - position);

        EmitVertex();
    }
    
    EndPrimitive();

    translate_matrix[3][0] = 1.5f;

    for (i=0; i < gl_in.length(); i++)
    {
        position = g_view_matrix[i]*translate_matrix*gl_in[i].gl_Position;

		gl_Position = proj_matrix*position;

		// Normal is already in camera space
		f_normal = g_normal[i];
		
		f_eye = -vec3(position);
		
		f_color = vec4(0.0, 0.0, 1.0, 1.0);

		// Light position is in camera space
		f_light_direction = vec3(light_position - position);

        EmitVertex();
    }
    
    EndPrimitive();
}

