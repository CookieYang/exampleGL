#version 450 core

#extension GL_ARB_bindless_texture : require

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec2 tc;
    flat uint instance_index;
}fs_in;

const vec3 ambient = vec3(0.1,0.1,0.1);
const vec3 diffuse_albedo = vec3(0.9,0.9,0.9);
const vec3 specular_albedo = vec3(0.7);
const float specular_power = 300.0;

layout (binding = 1, std140) uniform TEXTURE_BLOCK
{
    sampler2D tex[384];
};

void main(void)
{
 	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	vec3 H = normalize(L + V);

	vec3 diffuse = max(dot(N,L),0.0) * diffuse_albedo;
	diffuse *= texture(tex[fs_in.instance_index],fs_in.tc * 2.0).rgb;
	vec3 specular = pow(max(dot(N,H),0.0),specular_power) * specular_albedo;
	color = vec4(ambient + diffuse + specular, 1.0);
}