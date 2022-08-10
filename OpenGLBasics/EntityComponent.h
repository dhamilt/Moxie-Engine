#pragma once
#include "BaseObject.h"
static int component_id = 1;
class EntityComponent :
    public BaseObject
{
    
public:
    EntityComponent();
    ~EntityComponent();

    // Initialization function to be called by derived classes
    void Init();
    // public member functions
    // Returns the local to world space model matrix
    DMat4x4 GetLocalToWorldMatrix();
    // Returns the world space position of the component
    DVector3 GetWorldPosition();
    // Returns the local space position relative to the parent transform
    DVector3 GetRelativePosition();
    // Returns the rotation of the component in world space   
    quaternion GetWorldRotation();
    // Returns the rotation of the component relative to the parent transform
    quaternion GetRelativeRotation();
    // TODO: Create a way to store the number of each type of component
    // (possibly use a hashmap for this, but where to store it is the question),
    // but for now name is the name of the component + the incrementing id
    std::string GetName();
    // Toggles the component's Update functionality
    void SetTickable(bool isTickable);
    virtual void Update(double deltaTime) override;

    // Comparison operator
    bool operator==(const EntityComponent& other);

protected:
    std::string name = "EntityComponent" +std::to_string(component_id);        
    Transform* parent = nullptr;
    bool usesRenderData = false;

private:
    friend class MActor;
};

