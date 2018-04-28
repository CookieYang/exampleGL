#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;


uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out VS_OUT{
	vec4 color;
	vec2 texcoord;
}vs_out;

uniform vec3 light_pos = vec3(100.0,100.0,100.0);

uniform vec4 clip_plane = vec4(1.0,1.0,0.0,0.85);
uniform vec4 clip_sphere = vec4(0.0,0.0,0.0,4.0);


void main(void){
	gl_Position = proj_matrix * mv_matrix * position;
	vs_out.color = position * 2.0 + vec4(0.5,0.5,0.5,0.0);
	vs_out.texcoord = texcoord;
}

