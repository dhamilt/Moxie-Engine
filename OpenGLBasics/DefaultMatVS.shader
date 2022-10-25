#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;


out vec2 texCoord;
out vec3 normal;
out vec4 position;

uniform mat4 model; // Model matrix
uniform mat4 view; // View matrix
uniform mat4 projection; // Projection matrix

uniform mat3 normalMatrix; 

void main()
{	
	// Transform normal without perspective (projection matrix) added
	// and normalize it
	normal = normalize(normalMatrix * vertexNormal);
	position = view * model * vec4(vertexPosition, 1.0);
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}