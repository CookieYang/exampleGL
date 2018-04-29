#version 430 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out VS_OUT {
    vec3 color;
}vs_out;

uniform vec3 light_pos = vec3(100.0,100.0,100.0);
uniform vec3 diffuse_albedo = vec3(0.5,0.2,0.7);
uniform vec3 specular_albedo = vec3(0.7);
uniform vec3 ambient = vec3(0.1);
uniform float specular_power = 128.0f;

layout(std140) uniform constants {
  mat4 mv_matrix;
  mat4 view_matrix;
  mat4 proj_matrix;
};

void main(void){
  vec4 P = mv_matrix * position;

  vec3 N = normal * mat3(mv_matrix);

  vec3 L = light_pos - P.xyz;

  vec3 V = -P.xyz;

  N = normalize(N);
  L = normalize(L);
  V = normalize(V);

  vec3 R = reflect(-L,N);

  vec3 diffuse = diffuse_albedo * max(0.0,dot(L, N));
  vec3 specular = specular_albedo * pow(max(0.0, dot(R,V)),specular_power);

  vs_out.color = diffuse + specular + ambient;
  gl_Position = proj_matrix * P;
}

