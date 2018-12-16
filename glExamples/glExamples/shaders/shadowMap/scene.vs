#version 430 core
//out vec2 coord;
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform mat4 proj_matrix;
uniform mat4 light_matrix;
uniform vec3 light_position;

out vec4 f_color;
out vec3 f_normal;
out vec3 f_eye;
out vec4 shadowPosition;
out vec3 lightDirection;

void main(void){
/*const vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),                      
                                     vec4( 1.0, -1.0, 0.5, 1.0),                       
                                     vec4( 1.0,  1.0, 0.5, 1.0),                       
                                     vec4(-1.0,  1.0, 0.5, 1.0));
	coord = vertices[gl_VertexID].xy;
    gl_Position = vertices[gl_VertexID];*/
    
    shadowPosition = proj_matrix * light_matrix * model_matrix * position;
    f_normal = mat3(view_matrix * model_matrix) * normal;
    f_eye = -vec3(view_matrix * model_matrix * position);
    gl_Position = proj_matrix * view_matrix * model_matrix * position;
}


