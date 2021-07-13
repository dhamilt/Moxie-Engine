#pragma once
#include "EntityComponent.h"
#include "Mesh.h"
class MeshComponent :
    public EntityComponent
{
public:
    MeshComponent();
    ~MeshComponent() { if (mesh) delete mesh; }
    void InitializeMesh(std::vector<vertex> _vertices, std::vector<uint16_t> indices);
    virtual void Update(double deltaTime) override;
public:
    Mesh* mesh=nullptr;
};

