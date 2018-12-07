#version 430 core

layout (location = 0) in vec2 position;

uniform mat4 mvp_matrix;

uniform sampler2D particlesDataTexture;

out float lifeTime;

void main(void){
	vec4 P = mv_matrix * position;

	vs_out.N = mat3(mv_matrix) * normal;

	vs_out.L = light_pos - P.xyz;

	vs_out.V = -P.xyz;

	gl_ClipDistance[0] = dot(position, clip_plane);
	gl_ClipDistance[1] = length(position.xyz / position.w - clip_sphere.xyz) - clip_sphere.w;

	gl_Position = proj_matrix * P;
}
