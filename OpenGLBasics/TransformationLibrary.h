#pragma once
#include "BaseObject.h"
#include "glm/gtc/constants.hpp"
#include "glm/ext/quaternion_common.hpp"
#include <math.h>
class TransformationLibrary
{
public:
	static vector3 Lerp(const vector3& start, const vector3& end, const float& delta);
	static void CircleLerp(const vector3& axis, const float& radius, const float& delta, const vector3& origin, vector3& pos);	

	static void RotateTowards(Transform& transform, vector3 target);
	//static void RotateTowards(const vector3& targetLoc, const vector3& currentLoc, const vector3& currentDir, quaternion& currentRot);

};

inline vector3 TransformationLibrary::Lerp(const vector3& start, const vector3& end, const float& delta)
{
	return  start + (end - start) * delta;
}

inline void TransformationLibrary::CircleLerp(const vector3& axis, const float& radius, const float& delta, const vector3& origin, vector3& pos)
{		
	// two revolutions
	float twoPI = 3.141592f * 2;

	// The two vector components that make the circle
	float m = glm::sin(twoPI * delta) * radius;
	float n = glm::cos(twoPI * delta) * radius;

	// Have the position revolve around the y-axis
	// for initial position calculations
	vector3 result = vector3(m, 0, n);

	// Find the rotation from the y-axis to the arbituary axis
	vector3 axisDir = glm::normalize(axis);
	// The arbituary axis needed to rotate towards to desired position
	vector3 cross = glm::cross(vector3(0, 1, 0), axisDir);
	float angle = glm::acos(dot(axisDir, vector3(0, 1, 0)));
	quaternion rot = quaternion(angle, cross);
	// Offset the position based on the rotation 
	pos = result/* * rot*/;
}

inline void TransformationLibrary::RotateTowards(Transform& transform, vector3 target)
{
	vector3 forward = vector3(0.0f, 0.0f, 1.0f);
	vector3 currentLocation = transform.GetPosition();
	vector3 currentDirection = transform.GetForwardVector();

	// get the axis of rotation needed in order to rotate towards the target
	vector3 axisOfRotation = glm::cross(currentDirection, glm::normalize(target - currentLocation));

	// get the angle between the current direction and the desired
	float angleDefaultRot = glm::acos(glm::dot(forward, glm::normalize(target - currentLocation)));
	float angleCurrentRot = glm::acos(glm::dot(currentDirection, glm::normalize(target - currentLocation)));
	printf("\rAngle: %.2f", angleDefaultRot - angleCurrentRot);
	// Convert angle into degrees
	float angleInDeg = (angleDefaultRot- angleCurrentRot) /** 57.2958f*/;
	mat4 rot =  glm::rotate(mat4(1.0f), angleDefaultRot, axisOfRotation);
	transform.Rotate(axisOfRotation, angleInDeg);
	/*if( angle >= 1.0f)
		transform.SetRotation(rot);*/
}

//inline void TransformationLibrary::RotateTowards(const vector3& targetLoc, const vector3& currentLoc, const vector3& currentDir, quaternion& currentRot)
//{
//	// vector going towards the target from current location
//	vector3 dir = glm::normalize( targetLoc - currentLoc);
//	
//
//	float dot = glm::dot(currentDir, dir);
//
//	if (glm::abs(dot + 1) < 0.000001f)
//	{
//		currentRot = quaternion(0, 1, 0, glm::pi<float>());
//		return;
//	}
//
//	if (glm::abs(dot - 1) < 0.000001f)
//	{
//		currentRot = quaternion(0, 0, 0, 1);
//		return;
//	}
//	// retrieve the angle between the two vectors
//	float angle = glm::acos(dot) /2 /*- turn rate*/;
//
//	// retrieve the axis of rotation for the rotate action
//	vector3 axisOfRotation = glm::normalize(glm::cross(currentDir, dir));
//
//	vector3 rotXYZ = glm::sin(angle) * axisOfRotation;
//	quaternion targetRot = quaternion(rotXYZ.x, rotXYZ.y, rotXYZ.z, glm::cos(angle));
//	
//	// get the inverse of the target rotation
//	quaternion targetInv = glm::inverse(targetRot);
//	// retrieve the conjugate of the quaternion to find the correct rotation the current rotation needs to move to
//	currentRot = targetRot *  currentRot * targetInv;
//	/*vector3 localRight = currentRot * vector3(1, 0, 0);
//	vector3 localForward = currentRot * vector3(0, 0, 1);
//	mat4 result  =glm::lookAt(currentLoc, targetLoc, glm::cross(localRight, localForward));*/
//}
