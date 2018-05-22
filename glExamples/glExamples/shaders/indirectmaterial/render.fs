#version 430 core

layout (location = 0) out vec4 color;

// Input from vertex shader
in VS_OUT
{
    smooth vec3 N;
    smooth vec3 L;
    smooth vec3 V;
	flat int material_id;
} fs_in;

struct MaterialProperties
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

layout (binding = 2) uniform MATERIALS
{
	MaterialProperties material[100];
};


void main(void){
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);
	vec3 H = normalize(L + V);

	vec3 ambient = material[fs_in.material_id].ambient.rgb;
	vec3 specular_albedo = material[fs_in.material_id].specular.rgb;
	vec3 diffuse_albedo = material[fs_in.material_id].diffuse.rgb;
	float specular_power = material[fs_in.material_id].specular.a;


	vec3 diffuse = max(dot(N,L),0.0) * diffuse_albedo;
	vec3 specular = pow(max(dot(N,H),0.0),specular_power) * specular_albedo;

    color = vec4(ambient + diffuse + specular,1.0);

}

