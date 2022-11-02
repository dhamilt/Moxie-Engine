#pragma once
#include "glPCH.h"
class DVectorLibrary
{
public:
	static DVector3 GetMidPoint(const DVector3& A, const DVector3& B);
};


inline DVector3 DVectorLibrary::GetMidPoint(const DVector3& A, const DVector3& B)
{
	DVector3 dirAB = glm::normalize(B - A);
	float ABMag = glm::length(B - A);
	return A + (dirAB * ABMag / 2.0f);
}