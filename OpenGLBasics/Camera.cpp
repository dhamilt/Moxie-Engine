#include "glPCH.h"
#include "Camera.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;

vector3 Camera::GetRightVector()
{
    return glm::normalize(glm::cross(up, transform.GetForwardVector()));
    
}


mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForwardVector(), up);
}

void Camera::MoveCamera(MOX_Events movementOptions)
{
    vector3 dir = transform.GetForwardVector();
    const float cameraSpeed =0.2f; // TODO: find a way change this at runtime through input
    vector3 translationVector = vector3(0.0f);
   
    // if moving diagonal left and forward
    if( movementOptions == (MOX_FORWARD | MOX_LEFT))
        transform.Translate(glm::normalize(glm::cross(dir, up)) * -cameraSpeed + cameraSpeed * dir);        

	// if moving diagonal right and forward
    else if (movementOptions == (MOX_FORWARD | MOX_RIGHT))
		transform.Translate(glm::normalize(glm::cross(dir, up)) * cameraSpeed + cameraSpeed * dir);

	// if moving diagonal left and backward
	else if (movementOptions == (MOX_BACKWARD | MOX_LEFT))
		transform.Translate(glm::normalize(glm::cross(dir, up)) * -cameraSpeed + -cameraSpeed * dir);

	// if moving diagonal right and backward
	else if (movementOptions == (MOX_BACKWARD | MOX_RIGHT))
		transform.Translate(glm::normalize(glm::cross(dir, up)) * cameraSpeed + -cameraSpeed * dir);

	// if moving left
	else if (movementOptions == MOX_LEFT)
		transform.Translate(glm::normalize(glm::cross(dir, up)) * -cameraSpeed);

	// if moving right
	else if (movementOptions == MOX_RIGHT)
		transform.Translate(glm::normalize(glm::cross(dir, up)) * cameraSpeed);

	// if moving forward
	else if (movementOptions == MOX_FORWARD)
		transform.Translate(cameraSpeed * dir);

	// if moving backward
	else if (movementOptions == MOX_BACKWARD)
		transform.Translate(-cameraSpeed * dir);			
}

void Camera::RotateCamera(double xPos, double yPos)
{ 
    if (GGLSPtr->IsViewportInFocus())
    {
        if (firstFrameHeld)
        {
            p_xPos = xPos;
            p_yPos = yPos;

            firstFrameHeld = false;
        }


        float xOffset = float(p_xPos - xPos);
        float yOffset = float(p_yPos - yPos);

        p_xPos = xPos;
        p_yPos = yPos;

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw -= xOffset;
        pitch += yOffset;


        transform.Rotate(vector3(1.0f, 0.0f, 0.0f), glm::radians(yOffset));
        transform.Rotate(vector3(0.0f, 1.0f, 0.0f), glm::radians(-xOffset));
    }
}

void Camera::OnMouseButtonDown()
{
    if(GGLSPtr->IsViewportInFocus())
        firstFrameHeld = true;
}
