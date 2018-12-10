#version 430 core

layout (location = 0) out vec4 color;

in vec3 refractDir;
in vec3 reflectDir;
in float fresnel;

uniform samplerCube cubeTex;

void main(void){
	vec4 refractionColor = texture(cubeTex, normalize(refractDir));
	vec4 reflectionColor = texture(cubeTex, normalize(reflectDir));
		
	color = mix(refractionColor, reflectionColor, fresnel);
	
}