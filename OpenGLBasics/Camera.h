#pragma once
#include "glPCH.h"
#include "BaseObject.h"

class Camera: public BaseObject
{
public:
	Camera(): BaseObject(){	}
	Camera(const DVector3& pos) : BaseObject(pos) { }
	Camera(const DVector3& pos, const quaternion& quat) : BaseObject(pos)
	{
		float angle = glm::acos(quat.w) * 2;
		DVector3 aor(quat.x / glm::sin(angle / 2),
					quat.y / glm::sin(angle / 2),
					quat.z / glm::sin(angle / 2)); 

		transform.Rotate(aor, angle);
	}
	DVector3 GetRightVector();		
	DMat4x4 GetViewMatrix();
	void MoveCamera(MOX_Events movementOptions);
	void RotateCamera(double xPos, double yPos);
	void  OnMouseButtonDown(); // should be called for the first frame the mouse button was released
	

private:	
	DVector3 target = DVector3(0.0f, 0.0f, 1.0f);
	DVector3 up = DVector3(0.0f, 1.0f, 0.0f);
	double p_xPos = 0.0f, p_yPos = 0.0f;
	float sensitivity = 30.f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
	bool firstFrameHeld = true;
};

