#version 430 core

layout (location = 0) out vec4 color;

uniform samplerCube tex_cubemap;


// Input from vertex shader
in VS_OUT
{
	vec3 normal;
	vec3 view;
} fs_in;

void main(void){
	vec3 r = reflect(fs_in.view, normalize(fs_in.view));
	color = texture(tex_cubemap, r) * vec4(0.95,0.80,0.45,1.0);
}

