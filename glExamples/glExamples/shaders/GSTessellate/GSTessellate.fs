#version 430 core

flat in vec4 color;

out vec4 out_color;

void main(void){
	out_color = color;
	//out_color = vec4(1.0f,0.0f,0.0f,1.0f);
}

