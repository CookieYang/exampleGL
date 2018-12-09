#version 430 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 view_matrix;

out vec3 g_normal;
out mat4 g_view_matrix;

void main(void){
	
	g_normal = mat3(view_matrix) * normal;
    g_view_matrix = view_matrix;
    gl_Position = position;
}

