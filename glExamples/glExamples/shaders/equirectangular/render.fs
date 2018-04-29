#version 430 core

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D tex_envmap;


// Input from vertex shader
in VS_OUT
{
	vec3 normal;
	vec3 view;
} fs_in;

void main(void){
	vec3 u = normalize(fs_in.view);
	vec3 r = reflect(u, normalize(fs_in.normal));
	vec2 tc;
	tc.y = r.y; r.y = 0.0;
	tc.x = normalize(r).x * 0.5;
	float s = sign(r.z) * 0.5;
	tc.s = 0.75 - s * (0.5 - tc.s);
	tc.t = 0.5 + 0.5 * tc.t;
	color = texture(tex_envmap, tc);
}

