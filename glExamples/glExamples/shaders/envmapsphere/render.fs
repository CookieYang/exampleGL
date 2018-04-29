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
	r.z += 1.0;
	float m = 0.5 * inversesqrt(dot(r,r));
	color = texture(tex_envmap, r.xy * m + vec2(0.5));
	//color = vec4(1.0f,0.0f,0.0f,1.0f);
}

