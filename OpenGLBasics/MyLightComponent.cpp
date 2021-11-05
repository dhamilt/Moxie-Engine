#include "glPCH.h"
#include "MyLightComponent.h"

MyLightComponent::MyLightComponent()
{
	lightColor = Blue;
}

void MyLightComponent::Update(double deltaTime)
{
	// Call the base functionality first
	LightComponent::Update(deltaTime);
}
