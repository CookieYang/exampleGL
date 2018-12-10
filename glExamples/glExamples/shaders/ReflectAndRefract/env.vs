#version 430 core

layout (location = 0) in vec4 position;


uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 proj_matrix;

out vec3 f_ray;


void main(void){
 
	f_ray = normalize(position.xyz);

	gl_Position = proj_matrix * view_matrix * model_matrix * position;
}

