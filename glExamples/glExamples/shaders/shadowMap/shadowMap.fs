#version 430 core

layout (location = 0) out vec4 color;

uniform vec4 obj_color;

void main(void){
	color = obj_color;
}

