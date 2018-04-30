#version 430 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 4) in vec2 tc;

out VS_OUT {
    vec3 normal;
    vec3 view;
    vec2 tc;
}vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void){
	
	vec4 P = mv_matrix * position;
    vs_out.normal = mat3(mv_matrix) * normal;
    vs_out.view = P.xyz;
    vs_out.tc = tc;
    gl_Position = proj_matrix * P;
}

