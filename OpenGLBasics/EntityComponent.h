#pragma once
#include "BaseObject.h"
static int id = 0;
class EntityComponent :
    public BaseObject
{
    
public:
    EntityComponent();
    ~EntityComponent();

    // public member functions
    // Returns the local to world space model matrix
    mat4 GetLocalToWorldMatrix();
    // Returns the world space position of the component
    vector3 GetWorldPosition();
    // Returns the local space position relative to the parent transform
    vector3 GetRelativePosition();
    // Returns the rotation of the component in world space   
    quaternion GetWorldRotation();   
    // TODO: Create a way to store the number of each type of component
    // (possibly use a hashmap for this, but where to store it is the question),
    // but for now name is the name of the component + the incrementing id
    std::string GetName();

    // Comparison operator
    bool operator==(const EntityComponent& other);

protected:
        std::string name = "EntityComponent" +std::to_string(id);
        

    // private members
private:
    Transform* parent = nullptr;    

    friend class MActor;
};

