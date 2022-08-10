#pragma once
#include "Shader.h"
class Material
{
public:
	Shader* Get();
	void SetShader(Shader* _shader);
private:
	Shader* shader;
};

