#include "glPCH.h"
#include "EntityComponent.h"
#include "GLSetup.h"
extern GLSetup* GGLSPtr;
EntityComponent::EntityComponent()
{
    
}

EntityComponent::~EntityComponent()
{
    // remove reference to parent
    parent = nullptr;
}

void EntityComponent::Init()
{
   
    std::string message = name;
   
    message += " was created.";
    MLOG_MESSAGE(message.c_str());
}

DMat4x4 EntityComponent::GetLocalToWorldMatrix()
{
    return transform.GetTransform() * parent->GetTransform();
}

DVector3 EntityComponent::GetWorldPosition()
{
    // Convert the local model matrix (transform) of the component to world space coordinates
    DMat4x4 localToWorldMatrix = transform.GetTransform() * parent->GetTransform();
    return DVector3(localToWorldMatrix[0][3], localToWorldMatrix[1][3], localToWorldMatrix[2][3]);
}

DVector3 EntityComponent::GetRelativePosition()
{
    return transform.GetPosition();
}

// TODO: ADD THESE CALCULATIONS TO A MATH LIBRARY
quaternion EntityComponent::GetWorldRotation()
{
    // Convert the local model matrix (transform) of the component to world space coordinates
    DMat4x4 localToWorldMatrix = transform.GetTransform() * parent->GetTransform();
    // Get the world scale of the transform
    DVector3 worldScale = DVector3();    
    worldScale.x = glm::length(DVector3(localToWorldMatrix[0][0], localToWorldMatrix[1][0], localToWorldMatrix[2][0]));
    worldScale.y = glm::length(DVector3(localToWorldMatrix[0][1], localToWorldMatrix[1][1], localToWorldMatrix[2][1]));
    worldScale.z = glm::length(DVector3(localToWorldMatrix[0][2], localToWorldMatrix[1][2], localToWorldMatrix[2][2]));

    // Extract the rotation matrix from the local to world matrix
    // by dividing it by it's scale
    DMat4x4 _rotation = localToWorldMatrix;

    _rotation[0][0] /= worldScale.x;
    _rotation[1][0] /= worldScale.x;
    _rotation[2][0] /= worldScale.x;

    _rotation[0][1] /= worldScale.y;
    _rotation[1][1] /= worldScale.y;
    _rotation[2][1] /= worldScale.y;

    _rotation[0][2] /= worldScale.z;
    _rotation[1][2] /= worldScale.z;
    _rotation[1][2] /= worldScale.z;

    // Convert the matrix to a quaternion
    // retrieve the w component for the quaternion
    float qw = glm::sqrt(_rotation[0][0] + _rotation[1][1] + _rotation[2][2] + 1) / 2;
    // calculate the quaternion
    return quaternion((_rotation[2][1] - _rotation[1][2]) / (4 * qw),
        (_rotation[0][2] - _rotation[2][0]) / (4 * qw),
        (_rotation[2][1] - _rotation[1][2]) / (4 * qw),
        qw);    
}

quaternion EntityComponent::GetRelativeRotation()
{   
    return transform.GetRotation();
}

std::string EntityComponent::GetName()
{
    return name;
}

void EntityComponent::SetTickable(bool isTickable)
{
    hasTick = isTickable;
}

void EntityComponent::Update(double deltaTime)
{
    // if the component uses rendering
    if (usesRenderData)
    {
        // send model matrix data (transform) to the rendering pipeline
        if (GGLSPtr->GetPipeline())
            GGLSPtr->GetPipeline()->UpdateModelMatrix(transform.GetTransform(), name);
    }
}

bool EntityComponent::operator==(const EntityComponent& other)
{
    return this->name == other.name;
}
