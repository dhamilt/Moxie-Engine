#version 450 core

layout(location = 0) out vec4 fragColor;

layout(location = 1) in vec3 texCoords;

layout(binding = 2) uniform samplerCube skybox;


void main()
{
	fragColor = texture(skybox, texCoords);
}