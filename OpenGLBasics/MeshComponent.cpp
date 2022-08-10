#include "glPCH.h"
#include "MeshComponent.h"
#include "GLSetup.h"
extern GLSetup* GGLSPtr;
MeshComponent::MeshComponent()
{	
	name = "MeshComponent" + std::to_string(component_id);
	component_id++;
	usesRenderData = true;
}

void MeshComponent::AddMesh(Mesh* _mesh)
{
	mesh = _mesh;
	// update the rendering pipeline with the changes
	if (GGLSPtr)
		GGLSPtr->ImportMesh(_mesh);
}

void MeshComponent::SetMaterial(Material* mat)
{
	material = mat;
	if (GGLSPtr)
		GGLSPtr->AddMaterialToPipeline(mesh->GetMeshData()->name, mat);
}

void MeshComponent::InitializeMesh(std::vector<DVertex> _vertices, std::vector<uint16_t> indices)
{
	mesh = new Mesh(_vertices, indices);
}

void MeshComponent::Update(double deltaTime)
{
	EntityComponent::Update(deltaTime);
	// Update transform of the mesh before drawing
	this->mesh->UpdateModelMatrixPos(this->GetLocalToWorldMatrix());
}
