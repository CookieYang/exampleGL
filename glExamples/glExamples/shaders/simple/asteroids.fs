#version 430 core

in VS_OUT{
	vec3 normal;
	vec4 color;
}fs_in;

out vec4 FragColor;

void main(void){
	vec3 N = normalize(fs_in.normal);

	FragColor = fs_in.color * abs(N.z);
}

