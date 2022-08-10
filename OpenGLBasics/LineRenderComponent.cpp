#include "glPCH.h"
#include "LineRenderComponent.h"

LineRenderComponent::LineRenderComponent()
{
	name = "LineRenderComponent" + std::to_string(component_id);

	Init();
	component_id++;

	renderer = new LineRenderer();
}

void LineRenderComponent::Update(double deltaTime)
{
	EntityComponent::Update(deltaTime);
	// Update the line renderer with the current transform info
	renderer->cachedTransform = GetLocalToWorldMatrix();
}

void LineRenderComponent::SetLineWidth(float width)
{
	renderer->SetLineWidth(width);
}

int LineRenderComponent::GetPoints(std::vector<DVector3>& points)
{
	points = renderer->linePoints;

	return (int)points.size();
}
