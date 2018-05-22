#version 450 core

#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in vec3 position_3;
layout (location = 1) in vec3 normal;

#define MATRIX_ARRAYS

layout (std140, binding = 0) uniform FRAME_DATA
{
#ifdef MATRIX_ARRAYS
    mat4 m[3];
#else
    mat4 view_matrix;
    mat4 proj_matrix;
    mat4 viewproj_matrix;
#endif
};



out VS_OUT {
  smooth vec3 N;
  smooth vec3 L;
  smooth vec3 V;
  flat int material_id;
}vs_out;

uniform vec3 light_pos = vec3(100.0,50.0,100.0);

layout(std140, binding = 0) readonly buffer OBJECT_TRANSFORM {
    mat4 model_matrix[];
};

void main(void){
#ifdef MATRIX_ARRAYS
    mat4 view_matrix = m[0];
    mat4 proj_matrix = m[1];
    mat4 viewproj_matrix = m[2];
#endif
    vec4 position = vec4(position_3, 1.0);
    mat4 mv_matrix = view_matrix * model_matrix[gl_DrawIDARB];
    vec4 P = mv_matrix * position;
    vs_out.N = mat3(mv_matrix) * normal;
    vs_out.L = light_pos - P.xyz;
    vs_out.V = -P.xyz;
    vs_out.material_id = gl_BaseInstanceARB;
    gl_Position = proj_matrix * P;
}

