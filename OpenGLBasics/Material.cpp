#include "glPCH.h"
#include "Material.h"

Shader* Material::Get()
{
    return shader;
}

void Material::SetShader(Shader* _shader)
{
    if (shader)
        delete shader;
    shader = _shader;
}
