#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexColor;

out vec2 texCoord;
out vec4 objectColor;
out vec3 normal;
out vec4 position;

uniform mat4 mvpMatrix; // Model View Projection matrix (P*V*M)
uniform mat4 mvMatrix;	// transform matrix pre - perspective(V*M)
uniform mat4 normalMatrix; 

void main()
{
	objectColor = vertexColor;
	// Transform normal without perspective (projection matrix) added
	// and normalize it
	normal = normalize(normalMatrix * vertexNormal);
	Position = mvMatrix * vertexPosition;
	gl_Position = mvpMatrix * vertexPosition;
}