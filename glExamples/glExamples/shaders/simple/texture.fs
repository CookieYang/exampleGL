#version 400 core
out vec4 FragColor;
uniform sampler2D texture;
void main()
{
	FragColor = texelFetch(texture, ivec2(gl_FragCoord.xy), 0);
}

