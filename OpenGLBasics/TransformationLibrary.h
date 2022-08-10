#pragma once
#include "BaseObject.h"
#include "glm/gtc/constants.hpp"
#include "glm/ext/quaternion_common.hpp"
#include <math.h>
class TransformationLibrary
{
public:
	static DVector3 Lerp(const DVector3& start, const DVector3& end, const float& delta);
	static void CircleLerp(const DVector3& axis, const float& radius, const float& delta, const DVector3& origin, DVector3& pos);	

	static void RotateTowards(Transform& transform, DVector3 target);
	//static void RotateTowards(const DVector3& targetLoc, const DVector3& currentLoc, const DVector3& currentDir, quaternion& currentRot);

};

inline DVector3 TransformationLibrary::Lerp(const DVector3& start, const DVector3& end, const float& delta)
{
	return  start + (end - start) * delta;
}

inline void TransformationLibrary::CircleLerp(const DVector3& axis, const float& radius, const float& delta, const DVector3& origin, DVector3& pos)
{		
	// two revolutions
	float twoPI = 3.141592f * 2;

	// The two vector components that make the circle
	float m = glm::sin(twoPI * delta) * radius;
	float n = glm::cos(twoPI * delta) * radius;

	// Have the position revolve around the y-axis
	// for initial position calculations
	DVector3 result = DVector3(m, 0, n);

	// Find the rotation from the y-axis to the arbituary axis
	DVector3 axisDir = glm::normalize(axis);
	// The arbituary axis needed to rotate towards to desired position
	DVector3 cross = glm::cross(DVector3(0, 1, 0), axisDir);
	float angle = glm::acos(dot(axisDir, DVector3(0, 1, 0)));
	quaternion rot = quaternion(angle, cross);
	// Offset the position based on the rotation 
	pos = result/* * rot*/;
}

inline void TransformationLibrary::RotateTowards(Transform& transform, DVector3 target)
{
	DVector3 forward = DVector3(0.0f, 0.0f, 1.0f);
	DVector3 currentLocation = transform.GetPosition();
	DVector3 currentDirection = transform.GetForwardVector();

	// get the axis of rotation needed in order to rotate towards the target
	DVector3 axisOfRotation = glm::cross(currentDirection, glm::normalize(target - currentLocation));

	// get the angle between the current direction and the desired
	float angleDefaultRot = glm::acos(glm::dot(forward, glm::normalize(target - currentLocation)));
	float angleCurrentRot = glm::acos(glm::dot(currentDirection, glm::normalize(target - currentLocation)));
	printf("\rAngle: %.2f", angleDefaultRot - angleCurrentRot);
	// Convert angle into degrees
	float angleInDeg = (angleDefaultRot- angleCurrentRot) /** 57.2958f*/;
	DMat4x4 rot =  glm::rotate(DMat4x4(1.0f), angleDefaultRot, axisOfRotation);
	transform.Rotate(axisOfRotation, angleInDeg);
	/*if( angle >= 1.0f)
		transform.SetRotation(rot);*/
}

//inline void TransformationLibrary::RotateTowards(const DVector3& targetLoc, const DVector3& currentLoc, const DVector3& currentDir, quaternion& currentRot)
//{
//	// vector going towards the target from current location
//	DVector3 dir = glm::normalize( targetLoc - currentLoc);
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
//	DVector3 axisOfRotation = glm::normalize(glm::cross(currentDir, dir));
//
//	DVector3 rotXYZ = glm::sin(angle) * axisOfRotation;
//	quaternion targetRot = quaternion(rotXYZ.x, rotXYZ.y, rotXYZ.z, glm::cos(angle));
//	
//	// get the inverse of the target rotation
//	quaternion targetInv = glm::inverse(targetRot);
//	// retrieve the conjugate of the quaternion to find the correct rotation the current rotation needs to move to
//	currentRot = targetRot *  currentRot * targetInv;
//	/*DVector3 localRight = currentRot * DVector3(1, 0, 0);
//	DVector3 localForward = currentRot * DVector3(0, 0, 1);
//	DMat4x4 result  =glm::lookAt(currentLoc, targetLoc, glm::cross(localRight, localForward));*/
//}
