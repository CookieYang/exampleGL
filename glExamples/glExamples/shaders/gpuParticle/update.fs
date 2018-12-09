#version 430 core

in vec4 particleData;
out vec4 fag_particleData;


void main(void){
	fag_particleData = particleData;
}

