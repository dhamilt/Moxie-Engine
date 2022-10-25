#include "glPCH.h"
#include "Material.h"

Material::Material()
{
     color = Color(1.f, 0.84f, 0, 1.f);
}

Material::Material(Shader* _shader)
{
    shader = _shader;
}

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
