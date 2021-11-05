#version 330 core

in vec2 texCoord;
in vec3 fragmentColor;
out vec4 color;


uniform sampler2D myTexture;

void main()
{
	color = vec4(fragmentColor, 1) * texture(myTexture, texCoord);
}