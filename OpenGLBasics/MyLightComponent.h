#pragma once
#include "LightComponent.h"
class MyLightComponent :
    public LightComponent
{
public:
    MyLightComponent();
    virtual void Update(double deltaTime) override;
};

