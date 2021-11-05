#pragma once
#include "glPCH.h"
#include "EntityComponent.h"
class Entity :
    public BaseObject
{
public:
    Entity() {}

    private:
    std::vector<EntityComponent*> components;
    

};

