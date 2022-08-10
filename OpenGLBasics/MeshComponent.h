#pragma once
#include "EntityComponent.h"
#include "Mesh.h"
#include "Material.h"
class MeshComponent :
    public EntityComponent
{
public:
    MeshComponent();
    ~MeshComponent() { if (mesh) delete mesh; }
    void AddMesh(Mesh* _mesh);
    void SetMaterial(Material* mat);
    void InitializeMesh(std::vector<DVertex> _vertices, std::vector<uint16_t> indices);
    virtual void Update(double deltaTime) override;
private:
    Mesh* mesh=nullptr;
    Material* material;
};

