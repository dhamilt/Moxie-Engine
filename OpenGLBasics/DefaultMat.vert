#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;


layout(set = 0, binding = 0)uniform mvpBufferObject
{
	mat4 model; // Model matrix
	mat4 view; // View matrix
	mat4 projection; // Projection matrix
} mvpBuffer;

layout(set = 0, binding = 1)uniform normalBuf
{ 
	mat3 normalMatrix;
} normalInfo;

layout(location = 3)out vec3 normal;
layout(location = 4)out vec2 texCoord;
layout(location = 5)out vec4 position;

void main()
{	
	// Transform normal without perspective (projection matrix) added
	// and normalize it
	normal = normalize(normalInfo.normalMatrix * vertexNormal);
	position = mvpBuffer.view * mvpBuffer.model * vec4(vertexPosition, 1.0);
	texCoord = vertexTexCoord;
	gl_Position = mvpBuffer.projection * mvpBuffer.view * mvpBuffer.model * vec4(vertexPosition, 1.0);
}