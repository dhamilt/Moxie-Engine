#include "glPCH.h"
#include "LightComponent.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;
LightComponent::LightComponent()
{
	name = "LightComponent";
	if (component_id > 1)
		name += std::to_string(component_id);
	Init();
	component_id++;
}

LightComponent::LightComponent(LightTypes _lightType) : lightType(_lightType)
{
	name = "LightComponent";
	printf("%s was spawned!\n", name.c_str());
}

void LightComponent::Update(double deltaTime)
{
	EntityComponent::Update(deltaTime);
	// Pass the light info be rendered on meshes in the scene
	Light light = ToLight();
	GGLSPtr->UpdateLightingCollection(GetName(), &light);
}

Light LightComponent::ToLight()
{
	Light info = Light();
		info.radius = this->radius;
		info.type = (int)this->lightType;
		info.lightIntensity = this->intensity;
		info.lightColor = this->lightColor;
		info.position = this->GetWorldPosition();
		info.direction = this->GetWorldRotation() * DVector3(0,0,1);
	return info;
}
