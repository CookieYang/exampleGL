#version 430 core
out vec2 coord;
void main(void){
const vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),                      
                                     vec4( 1.0, -1.0, 0.5, 1.0),                       
                                     vec4( 1.0,  1.0, 0.5, 1.0),                       
                                     vec4(-1.0,  1.0, 0.5, 1.0));
	coord = vertices[gl_VertexID].xy;
    gl_Position = vertices[gl_VertexID];
}


