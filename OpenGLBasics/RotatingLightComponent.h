#pragma once
#include "LightComponent.h"
class RotatingLightComponent :
    public LightComponent
{
public:
    RotatingLightComponent() {};
    RotatingLightComponent(Transform* _target);
    virtual void Update(double deltaTime) override;

    void SetTarget(Transform* _target);    
    float revolutionsPerSecond = 0.05f;

private:
    // flag that determines if this light component
    // around a target location or an arbiturary 
    // point in space
    bool rotateAroundTarget = false;

    Transform* target = nullptr;
    double accumulatedDelta = 0.0f;
};

