#include "glPCH.h"
#include "MeshComponent.h"
#include "GLSetup.h"
extern GLSetup* GGLSPtr;
MeshComponent::MeshComponent()
{	
	name = "MeshComponent";
	if (component_id > 1)
		name += std::to_string(component_id);
	Init();
	component_id++;
	usesRenderData = true;
	Shader defaultShader;
	
	GGLSPtr->GetDefaultMeshShader(&defaultShader);
	if (&defaultShader)
		material = new Material(&defaultShader);
}

void MeshComponent::AddMesh(Mesh* _mesh)
{
	mesh = _mesh;
	// update the rendering pipeline with the changes
	if (GGLSPtr)
		GGLSPtr->ImportMesh(name, _mesh);
}

void MeshComponent::SetMaterial(Material* mat)
{
	material = mat;
	if (GGLSPtr)
		GGLSPtr->AddMaterialToPipeline(name, mat);
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
