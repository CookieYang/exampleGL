#version 430 core

in vec2 position;

uniform mat4 mvp_matrix;

uniform sampler2D particlesDataTexture;

out float lifeTime;

void main(void){
	vec4 particlePosition = vec4(texture(particlesDataTexture, position).xyz, 1.0);
	lifeTime = texture(particlesDataTexture, position).w;
	gl_PointSize = 10.0f;
	gl_Position = mvp_matrix * particlePosition;
}

