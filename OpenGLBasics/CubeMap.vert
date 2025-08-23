#version 450 core

layout(location = 0) in vec3 vPos;

layout(location = 2) out vec3 texCoords;

layout(set = 0, binding = 0) uniform ObjBuf{ mat4 projection; mat4 view; } objProp;

void main()
{
	texCoords = vPos;
	gl_Position = objProp.projection * objProp.view * vec4(vPos, 1.0);
}