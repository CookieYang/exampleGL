#version 430 core

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D tex_color;
layout (binding = 1) uniform sampler2D tex_normal;

// Input from vertex shader
in VS_OUT
{
	vec3 eyeDir;
    vec3 lightDir;
	vec3 normal;
	vec2 texcoord;
} fs_in;

void main(void){
	vec3 N = normalize(texture(tex_normal, fs_in.texcoord).rgb * 2.0 - vec3(1.0));
	vec3 L = normalize(fs_in.lightDir);
	vec3 V = normalize(fs_in.eyeDir);

	vec3 R = reflect(-L, N);

	vec3 diffuse = max(dot(N,L),0.0) * texture(tex_color,fs_in.texcoord).rgb;
	vec3 specular = max(pow(dot(R,V),20.0),0.0) * vec3(1.0);

    color = vec4(diffuse + specular,1.0);
 
}

