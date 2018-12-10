#version 430 core

in vec3 f_ray;
uniform samplerCube cubeTex;
out vec4 color;

void main(void){
	color = texture(cubeTex, f_ray);
}

