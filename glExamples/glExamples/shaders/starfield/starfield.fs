#version 430 core

layout (location = 0) out vec4 color;

uniform sampler2D tex_star;
flat in vec4 starColor;

void main(void)
{
 	color = starColor * texture(tex_star, gl_PointCoord);
	//color = vec4(1.0,0.0,0.0,1.0);
}