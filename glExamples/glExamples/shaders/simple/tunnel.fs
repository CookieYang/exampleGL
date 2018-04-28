#version 430 core
out vec4 FragColor;
in VS_OUT {
vec2 tc;
}fs_in;
layout (binding = 0) uniform sampler2D tex;
void main()
{
	FragColor = texture(tex, fs_in.tc);
}

