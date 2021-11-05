#vertex
#version 330 core													

layout(location = 0) in vec3 pos;									

void main()															
{																	
gl_Position = vec4(pos.xyz * 0.5, 1.0);	
}

#fragment
#version 330 core

out vec4 color;

void main()
{
color = vec4(0.65, 0.3, 0.6, 1.0);
}