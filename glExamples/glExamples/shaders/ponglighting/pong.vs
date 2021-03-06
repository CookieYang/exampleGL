#version 430 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out VS_OUT {
    vec3 N;
    vec3 L;
    vec3 V;
}vs_out;

uniform vec3 light_pos = vec3(100.0,100.0,100.0);

layout(std140) uniform constants {
    mat4 mv_matrix;
    mat4 view_matrix;
    mat4 proj_matrix;
};

void main(void){
	
	vec4 P = mv_matrix * position;
	vs_out.N = mat3(mv_matrix) * normal;
	vs_out.L = light_pos - P.xyz;
	vs_out.V = -P.xyz;
	gl_Position = proj_matrix * P;
}

