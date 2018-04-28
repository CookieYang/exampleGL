#version 430 core
out vec4 FragColor;
in vec2 tex_coord;
layout (binding = 0) uniform sampler2D tex;
void main()
{
	FragColor = texture(tex, tex_coord);
}

