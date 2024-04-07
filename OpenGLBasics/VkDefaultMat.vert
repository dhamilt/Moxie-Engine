#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;


layout(location = 3)out vec2 texCoord;
layout(location = 4)out vec4 normal;
layout(location = 5)out vec4 position;

layout(std140, set = 0, binding = 0)uniform mvpBuf
{
	mat4 model; // Model matrix
	mat4 view; // View matrix
	mat4 projection; // Projection matrix
} mvpInfo;

layout(std140, set = 0, binding = 1)uniform normalBuf
{ 
	mat3 normalMatrix;
} normalInfo;

void main()
{	
	// Transform normal without perspective (projection matrix) added
	// and normalize it
	normal = vec4(normalize(normalInfo.normalMatrix * vertexNormal),1.0);
	position = mvpInfo.view * mvpInfo.model * vec4(vertexPosition, 1.0);
	gl_Position = mvpInfo.projection * mvpInfo.view * mvpInfo.model * vec4(vertexPosition, 1.0);
}