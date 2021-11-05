#pragma once
#include "EntityComponent.h"
#include "Color.h"
#include "Light.h"
class LightComponent :
    public EntityComponent
{
public:
    enum class LightTypes { Point, Directional, Spot };
    LightComponent();
    LightComponent(LightTypes _lightType);
    LightTypes lightType = LightTypes::Directional;
    float intensity = 1.0f;
    float radius = 5.0f;
    float linearLighting = 1.0f;

    MLinearColor lightColor = Red;
    virtual void Update(double deltaTime) override;
    // Converts the current light info on this component 
    // into a Light struct in order to be sent to the "rendering thread"
    // to be used draw calls
private:
    Light ToLight();
};

