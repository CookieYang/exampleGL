#version 430 core

layout(binding = 0) uniform sampler2DArray tex_array;

out vec4 out_color;

in VS_OUT {
    vec3 tc;
}fs_in;

void main(void){
	out_color = texture(tex_array,fs_in.tc);
}

