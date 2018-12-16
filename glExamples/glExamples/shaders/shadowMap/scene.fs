#version 430 core

layout (location = 0) out vec4 color;
//in vec2 coord;
uniform sampler2DShadow depthTex;
uniform vec4 obj_color;

in vec4 f_color;
in vec3 f_normal;
in vec3 f_eye;
in vec4 shadowPosition;
in vec3 lightDirection;

void main(void){
	/*vec3 projCoords = shadowPosition.xyz / shadowPosition.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(depthTex, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	//color = 100 * (vec4(1.0f) - texture(depthTex,coord)); */
	if (textureProj(depthTex, shadowPosition) < 1.0)
	//if (abs(shadow - 0.0) < 0.1)
	{
		color = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	} else {
		color = obj_color;
	}
	//color = shadowPosition;
}

