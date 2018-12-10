#version 430 core

// Indices of refraction
const float Air = 1.0;
const float Glass = 1.51714;

const float Eta = Air / Glass;

const float R0 = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass));

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 proj_matrix;
uniform vec4 view_position;

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out vec3 refractDir;
out vec3 reflectDir;
out float fresnel;

void main(void){
	vec4 worldPosition = model_matrix * position;
	vec3 incident = normalize(vec3(worldPosition-view_position));
	vec3 v_normal = normalize(mat3(model_matrix) * normal);
	refractDir = refract(incident, v_normal, Eta);
	reflectDir = reflect(incident, v_normal);
	fresnel = R0 + (1.0 - R0) * pow((1.0 - dot(-incident, v_normal)), 5.0);
	gl_Position = proj_matrix * view_matrix  * worldPosition;
}

