#pragma once
#include "glPCH.h"
#include "BaseObject.h"

class Camera: public BaseObject
{
public:
	Camera(): BaseObject(){	}
	Camera(const vector3& pos) : BaseObject(pos) { }
	Camera(const vector3& pos, const quaternion& quat) : BaseObject(pos)
	{
		float angle = glm::acos(quat.w) * 2;
		vector3 aor(quat.x / glm::sin(angle / 2),
					quat.y / glm::sin(angle / 2),
					quat.z / glm::sin(angle / 2)); 

		transform.Rotate(aor, angle);
	}
	vector3 GetRightVector();		
	mat4 GetViewMatrix();
	void MoveCamera(MOX_Events movementOptions);
	void RotateCamera(double xPos, double yPos);
	void  OnMouseUp(); // should be called for the first frame the mouse button was released
	

private:	
	vector3 target = vector3(0.0f, 0.0f, 1.0f);
	vector3 up = vector3(0.0f, 1.0f, 0.0f);
	double p_xPos = 0.0f, p_yPos = 0.0f;
	float sensitivity = 30.f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
	bool firstFrameHeld = true;
};

