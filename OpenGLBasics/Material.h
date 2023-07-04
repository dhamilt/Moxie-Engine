#pragma once
#include "Shader.h"
#include "VulkanShaders.h"
#include "Color.h"
class Material
{
public:
	Material();
	Material(Shader* _shader);
	Shader* Get();
	void SetShader(Shader* _shader);
	
public:
	Color color;


private:
	Shader* shader;
};

