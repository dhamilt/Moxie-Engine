#include "glPCH.h"
#include "RotatingLightComponent.h"
#include "TransformationLibrary.h"

RotatingLightComponent::RotatingLightComponent(Transform* _target)
{
	if (target)
		*target = *_target;
	else
		target = _target;
}

void RotatingLightComponent::Update(double deltaTime)
{
	LightComponent::Update(deltaTime);

	accumulatedDelta + deltaTime * revolutionsPerSecond >= 1.0f?accumulatedDelta = 0.0f : accumulatedDelta+=deltaTime*revolutionsPerSecond;	
	
	DVector3 pos = DVector3(1);
	// have the light go in a circle around the y axis
	TransformationLibrary::CircleLerp(DVector3(0.0f, 1.0f, 0.0f), radius, float(accumulatedDelta), DVector3(0.0f), pos);	

	transform.SetPosition(pos);
	//TransformationLibrary::RotateTowards(this->transform, DVector3(0));
	
}

void RotatingLightComponent::SetTarget(Transform* _target)
{
	if (target)
		*target = *_target;
	else
		target = _target;
}
