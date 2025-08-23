#version 450 core

layout(location = 0) out vec4 fragColor;

layout(location = 2) in vec3 texCoords;

layout(set = 1, binding = 2) uniform samplerCube skybox;


void main()
{
	fragColor = texture(skybox, texCoords) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
}