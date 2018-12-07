#version 430 core

layout (location = 0) out vec4 color;

in float lifeTime;

uniform sampler2D particleTexture;

void main(void){
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	vec3 R = reflect(-L,N);
	vec3 diffuse = max(dot(N,L),0.0) * diffuse_albedo;
	vec3 specular = pow(max(dot(R,V),0.0),specular_power) * specular_albedo;
	vec3 rim = calculate_rim(N,V);

	color = vec4(diffuse + specular + rim, 1.0);
}

