#pragma once
#include "BaseObject.h"
#include "EntityComponent.h"
class MActor :
    public BaseObject
{
    
public:
    void AddComponent(EntityComponent* component);
    void RemoveComponent(EntityComponent* component);

private:
    std::vector<EntityComponent*> components;
};

