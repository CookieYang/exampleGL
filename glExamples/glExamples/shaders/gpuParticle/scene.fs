#version 430 core

layout (location = 0) out vec4 color;

in float lifeTime;

uniform sampler2D particleTexture;

void main(void){
	color = texture(particleTexture, gl_PointCoord) * vec4(1.0f, 0.0f, 0.0f, lifeTime);
}
