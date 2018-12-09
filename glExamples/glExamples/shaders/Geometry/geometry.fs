#version 430 core

layout (location = 0) out vec4 fragColor;

in vec3 f_normal;
in vec3 f_eye;
in vec4 f_color;
in vec3 f_light_direction;


void main(void){
		// Note: All calculations are in camera space.

	vec4 color = 0.3 * f_color;

	vec3 normal = normalize(f_normal);
	
	vec3 lightDirection = normalize(f_light_direction);

	float nDotL = max(dot(lightDirection, normal), 0.0);
		
	if (nDotL > 0.0)
	{
		vec3 eye = normalize(f_eye);
	
		// Incident vector is opposite light direction vector.
		vec3 reflection = reflect(-lightDirection, normal);
		
		float eDotR = max(dot(eye, reflection), 0.0);
	
		color += 1.0 * f_color * nDotL;
		
		color += 1.0 * vec4(1.0, 1.0, 1.0, 1.0) * pow(eDotR, 20.0);
	}

	fragColor = color;

}

