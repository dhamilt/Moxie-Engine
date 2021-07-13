#pragma once
#include "glPCH.h"


struct Transform
{
public:
	Transform() { _translation = mat4(1.0f); _rotation = mat4(1.0f); _scaling = mat4(1.0f); }
	Transform(vector3 pos) { _translation = mat4(1.0f); SetPosition(pos); _rotation = mat4(1.0f); _scaling = mat4(1.0f); }
	Transform(vector3 pos, vector3 scale) { SetPosition(pos); _rotation = mat4(1.0f);  SetScale(scale); }
	Transform(vector3 pos, vector3 aor, float angle, vector3 scale) { Translate(pos); Rotate(aor, angle); Scale(scale); }
	vector3 GetPosition();
	void SetPosition(const vector3& pos);
	void Translate(const vector3& translation);
	quaternion GetRotation();
	mat4 GetRotationMatrix();
	void SetRotation(const quaternion& rotation);
	void SetRotation(const mat4& rotation);
	void Rotate(const vector3& axisOfRotation, const float& angle);
	//void Rotate(quaternion& rot);
	vector3 GetScale();
	void SetScale(const vector3& scale);
	void Scale(const vector3& scale);
	void Scale(const float& scaleFactor);
	mat4 GetTransform();
	vector3 GetForwardVector();
	void RotateTowards(vector3 target);
	void PrintTranslation();
	void Print();
	static void PrintMatrix(const mat4& matrix);
	
public:
	mat4 _translation;
	mat4 _rotation;
	mat4 _scaling;
};


class BaseObject
{	

public:	
	BaseObject();	
	BaseObject(vector3 pos);
	BaseObject(vector3 pos, vector3 scale);
	BaseObject(vector3 pos, vector3 aor, float angle, vector3 scale); 

	
	virtual void Update(double deltaTime) { timePassed += deltaTime; /*printf("\r Time: %.2f", timePassed);*/ };
public:
	double timePassed = 0;
	Transform transform;
	
};

inline vector3 Transform::GetPosition()
{
	return vector3(_translation[3][0], _translation[3][1], _translation[3][2]);
}

inline void Transform::SetPosition(const vector3& pos)
{
	/*_translation[3][0] *= pos.x;
	_translation[3][1] *= pos.y;
	_translation[3][2] *= pos.z;*/

	_translation = glm::translate(mat4(1.0f), pos);
}


inline void Transform::Translate(const vector3& translation)
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

inline mat4 Transform::GetRotationMatrix()
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

inline void Transform::SetRotation(const mat4& rotation)
{
	_rotation = rotation;
}

inline void Transform::Rotate(const vector3& axisOfRotation, const float& angle)
{
	_rotation *= glm::rotate(mat4(1.0f), angle, axisOfRotation);
}

//inline void Transform::Rotate( quaternion& rot)
//{
//	_rotation *= rot;
//}

inline vector3 Transform::GetScale()
{
	return vector3(_scaling[0][0], _scaling[1][1], _scaling[2][2]);
}

inline void Transform::SetScale(const vector3& scale)
{
	_scaling = glm::scale(mat4(1.0f), scale);
}

// scales the matrix by the parameter vector3 value
inline void Transform::Scale(const vector3& scale)
{
	_scaling *= glm::scale(mat4(1.0f), scale);
}

inline void Transform::Scale(const float& scaleFactor)
{
	_scaling *= scale(mat4(1), vector3(1)*scaleFactor);
}

inline mat4 Transform::GetTransform()
{
	return _translation* _rotation * _scaling;
}

inline vector3 Transform::GetForwardVector()
{
	return normalize(GetRotation() * vector3(0, 0, 1));
}

inline void Transform::RotateTowards(vector3 target)
{
	vector3 loc = GetPosition();
	// if the target's position and the current object's position are not the same
	if (target != loc)
	{
		vector4 dirPos = _rotation *  vector4(0.0f, 0.0f, 1.0f, 0.0f);
		vector3 dir = vector3(dirPos.x, dirPos.y, dirPos.z);

		// get the axis of rotation needed in order to rotate towards the target
		vector3 axisOfRotation = glm::cross(dir, glm::normalize(target - loc));

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
	mat4 _transform = GetTransform();
	printf("\r||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||",
		_transform[0][0], _transform[0][1], _transform[0][2], _transform[0][3],
		_transform[1][0], _transform[1][1], _transform[1][2], _transform[1][3],
		_transform[2][0], _transform[2][1], _transform[2][2], _transform[2][3],
		_transform[3][0], _transform[3][1], _transform[3][2], _transform[3][3]);
}

inline void Transform::PrintMatrix(const mat4& matrix)
{
	printf("\r||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||%.2f %.2f %.2f %.2f||",
		matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
		matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
}

