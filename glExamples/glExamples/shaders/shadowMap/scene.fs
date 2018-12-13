#version 430 core

layout (location = 0) out vec4 color;
in vec2 coord;
uniform sampler2D depthTex;

void main(void){
	color = 100 * (vec4(1.0f) - texture(depthTex,coord));
}

