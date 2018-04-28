#version 450 core

#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 4) in vec2 tc;

layout (std140, binding = 0) uniform MATRIX_BLOCK
{
    mat4 model_matrix[386];
};

out VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 tc;
    flat uint instance_index;
}vs_out;

const vec3 light_pos = vec3(100.0, 100.0, 100.0);

void main(void)
{
	mat4 view_matrix = model_matrix[0];
	mat4 proj_matrix = model_matrix[1];

	mat4 mv_matrix = view_matrix * model_matrix[gl_InstanceID + 2];

	vec4 P = mv_matrix * position;
	vs_out.N = mat3(mv_matrix) * normal;
	vs_out.L = light_pos - P.xyz;
	vs_out.V = -P.xyz;
	vs_out.tc = tc * vec2(5.0,1.0);
	vs_out.instance_index = gl_InstanceID;
	gl_Position = proj_matrix * P;
}
