#version 430 core
out vec4 FragColor;
in VS_OUT {
flat int alien;
vec2 tc;
}fs_in;
uniform sampler2DArray tex_aliens;
void main()
{
	FragColor = texture(tex_aliens, vec3(fs_in.tc, float(fs_in.alien)));
}

