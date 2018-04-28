#version 430 core

in GS_OUT{
    vec4 color;
    vec3 normal;
}fs_in;

out vec4 out_color;

void main(void){
	out_color = vec4(abs(fs_in.normal.z)) * fs_in.color;
	
}

