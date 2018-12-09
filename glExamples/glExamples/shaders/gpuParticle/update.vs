#version 430 core

in vec2 position;

uniform sampler2D particleDataTex;
uniform float time;
uniform int Width;

out vec4 particleData;

float rand(vec2 seed)
{
	return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(void){

	float position_speed = 0.2 * time;
	float lifetime_speed = 0.04;

	// Get particle data from last texture
	particleData = texture(particleDataTex, position);

	// Each particle has the same translation vector. Pseudo random function is used to randomly distribute the particles over the screen
	particleData.x += (rand(position*123.456)*2.0-1.0)*position_speed;
	particleData.y += (rand(position*234.567)*2.0-1.0)*position_speed;
	particleData.z += (rand(position*345.678)*2.0-1.0)*position_speed;
	
	// Decrease the lifetime
	particleData.w -= lifetime_speed;

	// Particle died, create new one
	if (particleData.w < 0.0)
	{
		particleData.x = 0.0f;
		particleData.y = 0.0f;
		particleData.z = 0.0f;
		particleData.w = clamp(1.0 - rand(position*456.789), 0.0, 1.0);
	}

	gl_PointSize = 1.0f;

	gl_Position = vec4(-vec2(1.0-1.0/Width, 1.0-1.0/Width) + position*2.0, 0.0, 1.0);
}

