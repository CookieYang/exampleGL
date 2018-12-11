#version 430 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;


void main(void){
	
	vec4 P = view_matrix * model_matrix * position;

	gl_Position = proj_matrix * P;
}

