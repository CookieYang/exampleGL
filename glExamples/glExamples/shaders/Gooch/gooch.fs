#version 430 core

layout (location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;


uniform vec3 surfaceColor = vec3(1.0f, 0.0f, 0.0f);
uniform vec3 hightColor = vec3(1.0f);

void main(void){
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);

	vec3 cool = vec3(0.0f, 0.0f, 0.55f) + 0.25 * surfaceColor;
	vec3 warm = vec3(0.3f, 0.3f, 0.0f) + 0.25 * surfaceColor;

	float dotNL = dot(N,L);
	float t = (dotNL + 1.0f) / 2.0f;
	vec3 r = 2 * dotNL * N - L;
	float s = clamp((100 * dot(r, V) - 97), 0, 1);
	color = vec4(s * hightColor + (1 - s) * (t * warm + (1 - t) * cool), 1.0f);
}

