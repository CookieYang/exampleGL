#version 430 core
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main(void){
	const vec4 vertices[] = vec4[](vec4(0.4,-0.4,0.5,1.0),vec4(-0.4,-0.4,0.5,1.0),vec4(0.4,0.4,0.5,1.0),vec4(-0.4,0.4,0.4,1.0));

	gl_Position = proj_matrix * view_matrix * model_matrix * vertices[gl_VertexID];
}

