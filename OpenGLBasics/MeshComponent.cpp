#include "glPCH.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent()
{	
	name = "MeshComponent" + std::to_string(id);
	Init();
	id++;
}

void MeshComponent::InitializeMesh(std::vector<vertex> _vertices, std::vector<uint16_t> indices)
{
	mesh = new Mesh(_vertices, indices);
}

void MeshComponent::Update(double deltaTime)
{
	EntityComponent::Update(deltaTime);
	// Update transform of the mesh before drawing
	this->mesh->UpdateModelMatrixPos(this->GetLocalToWorldMatrix());
}
