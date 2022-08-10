#pragma once
#include "glPCH.h"


struct Transform
{
public:
	Transform() { _translation = DMat4x4(1.0f); _rotation = DMat4x4(1.0f); _scaling = DMat4x4(1.0f); }
	Transform(DVector3 pos) { _translation = DMat4x4(1.0f); SetPosition(pos); _rotation = DMat4x4(1.0f); _scaling = DMat4x4(1.0f); }
	Transform(DVector3 pos, DVector3 scale) { SetPosition(pos); _rotation = DMat4x4(1.0f);  SetScale(scale); }
	Transform(DVector3 pos, DVector3 aor, float angle, DVector3 scale) { Translate(pos); Rotate(aor, angle); Scale(scale); }
	DVector3 GetPosition();
	void SetPosition(const DVector3& pos);
	void Translate(const DVector3& translation);
	quaternion GetRotation();
	DMat4x4 GetRotationMatrix();
	void SetRotation(const quaternion& rotation);
	void SetRotation(const DMat4x4& rotation);
	void Rotate(const DVector3& axisOfRotation, const float& angle);
	//void Rotate(quaternion& rot);
	DVector3 GetScale();
	void SetScale(const DVector3& scale);
	void Scale(const DVector3& scale);
	void Scale(const float& scaleFactor);
	DMat4x4 GetTransform();
	DVector3 GetForwardVector();
	void RotateTowards(DVector3 target);
	void PrintTranslation();
	void Print();
	static void PrintMatrix(const DMat4x4& matrix);
	
public:
	DMat4x4 _translation;
	DMat4x4 _rotation;
	DMat4x4 _scaling;
};


class BaseObject
{	

public:	
	BaseObject();	
	BaseObject(DVector3 pos);
	BaseObject(DVector3 pos, DVector3 scale);
	BaseObject(DVector3 pos, DVector3 aor, float angle, DVector3 scale); 

	
	virtual void Update(double deltaTime) { timePassed += deltaTime; /*printf("\r Time: %.2f", timePassed);*/ };
public:
	double timePassed = 0;
	Transform transform;

protected:
	bool hasTick = true;
	
	friend class GameLoop;
};

inline DVector3 Transform::GetPosition()
{
	return DVector3(_translation[3][0], _translation[3][1], _translation[3][2]);
}

inline void Transform::SetPosition(const DVector3& pos)
{
	/*_translation[3][0] *= pos.x;
	_translation[3][1] *= pos.y;
	_translation[3][2] *= pos.z;*/

	_translation = glm::translate(DMat4x4(1.0f), pos);
}


inline void Transform::Translate(const DVector3& translation)
{
	_translation = glm::translate(_translation, translation);
}

inline quaternion Transform::GetRotation()
{	
	// retrieve the w component for the quaternion
	float qw = glm::sqrt(_rotation[0][0] + _rotation[1][1] + _rotation[2][2] + 1) / 2;
	// calculate the quaternion
	return quaternion((_rotation[2][1] - _rotation[1][2]) / (4* qw),
		(_rotation[0][2] - _rotation[2][0]) / (4 * qw),
		(_rotation[2][1] - _rotation[1][2]) / (4 * qw),
			qw);
}

inline DMat4x4 Transform::GetRotationMatrix()
{
	return _rotation;
}

inline void Transform::SetRotation(const quaternion& rotation)
{
	float qx = rotation.x,
		qy = rotation.y,
		qz = rotation.z,
		qw = rotation.w;

	_rotation[0][0] = 1 - 2 * (qy * qy) - 2 * (qz * qz);
	_rotation[0][1] = 2 * qx * qy - 2 * qz * qw;
	_rotation[0][2] = 2 * qx * qz + 2 * qy * qw;

	_rotation[1][0] = 2 * qx * qy + 2 * qz * qw;
	_rotation[1][1] = 1 - 2 * (qx * qx) - 2 * (qz * qz);
	_rotation[1][2] = 2 * (qy * qz) - 2 * (qx * qw);

	_rotation[2][0] = 2 * (qx * qz) - 2 * (qy * qw);
	_rotation[2][1] = 2 * (qy * qz) + 2 * (qx * qw);
	_rotation[2][2] = 1 - 2 * (qx * qx) - 2 * (qy * qy);
}

inline void Transform::SetRotation(const DMat4x4& rotation)
{
	_rotation = rotation;
}

inline void Transform::Rotate(const DVector3& axisOfRotation, const float& angle)
{
	_rotation *= glm::rotate(DMat4x4(1.0f), angle, axisOfRotation);
}

//inline void Transform::Rotate( quaternion& rot)
//{
//	_rotation *= rot;
//}

inline DVector3 Transform::GetScale()
{
	return DVector3(_scaling[0][0], _scaling[1][1], _scaling[2][2]);
}

inline void Transform::SetScale(const DVector3& scale)
{
	_scaling = glm::scale(DMat4x4(1.0f), scale);
}

// scales the matrix by the parameter DVector3 value
inline void Transform::Scale(const DVector3& scale)
{
	_scaling *= glm::scale(DMat4x4(1.0f), scale);
}

inline void Transform::Scale(const float& scaleFactor)
{
	_scaling *= scale(DMat4x4(1), DVector3(1)*scaleFactor);
}

inline DMat4x4 Transform::GetTransform()
{
	return _translation* _rotation * _scaling;
}

inline DVector3 Transform::GetForwardVector()
{
	return normalize(GetRotation() * DVector3(0, 0, 1));
}

inline void Transform::RotateTowards(DVector3 target)
{
	DVector3 loc = GetPosition();
	// if the target's position and the current object's position are not the same
	if (target != loc)
	{
		DVector4 dirPos = _rotation *  DVector4(0.0f, 0.0f, 1.0f, 0.0f);
		DVector3 dir = DVector3(dirPos.x, dirPos.y, dirPos.z);

		// get the axis of rotation needed in order to rotate towards the target
		DVector3 axisOfRotation = glm::cross(dir, glm::normalize(target - loc));

		// get the angle between the current direction and the desired
		float angle = glm::acos(glm::dot(dir, glm::normalize(target - loc)));
		if(angle * 57.2958f > 1.0f)
			Rotate(axisOfRotation, angle);
	}
	
}

inline void Transform::PrintTranslation()
{
	PrintMatrix(_translation);
}

inline void Transform::Print()
{
	DMat4x4 _transform = GetTransform();
	printf("\r||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||",
		_transform[0][0], _transform[0][1], _transform[0][2], _transform[0][3],
		_transform[1][0], _transform[1][1], _transform[1][2], _transform[1][3],
		_transform[2][0], _transform[2][1], _transform[2][2], _transform[2][3],
		_transform[3][0], _transform[3][1], _transform[3][2], _transform[3][3]);
}

inline void Transform::PrintMatrix(const DMat4x4& matrix)
{
	printf("\r||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||",
		matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
		matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
}

