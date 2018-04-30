#version 430 core

layout (location = 0) out vec4 color;

uniform samplerCube tex_cubemap;


// Input from vertex shader
in VS_OUT
{
	vec3 tc;
} fs_in;

void main(void){
	color = texture(tex_cubemap, fs_in.tc);
	//color = vec4(1.0f,0.0f,0.0f,1.0f);
}

