#pragma once
#include "glPCH.h"

struct Color
{
	Color()
		:
		r(0.0f)
	,	g(0.0f)
	,	b(0.0f)
	,	a(0.0f)
	{}

	Color(float _r, float _g, float _b, float _a)
		:
		r(_r)
	,	g(_g)
	,	b(_b)
	,	a(_a)
	{
		ClampValues();
	}
	DVector3 rgb() { return DVector3(r, g, b); };
	DVector4 rgba() { return DVector4(r, g, b, a); };

	void ClampValues();
	float r, g, b, a;
	
	
};


inline void Color::ClampValues()
{
	this->r = glm::clamp(this->r, 0.0f, 1.0f);
	this->g = glm::clamp(this->g, 0.0f, 1.0f);
	this->b = glm::clamp(this->b, 0.0f, 1.0f);
	this->a = glm::clamp(this->a, 0.0f, 1.0f);
}

static const Color Blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
static const Color Green = Color(0.0f, 1.0f, 0.0f, 1.0f);
static const Color Red = Color(1.0f, 0.0f, 0.0f, 1.0f);
static const Color Purple = Color(1.0f, 0.0f, 1.0f, 1.0f);
static const Color Yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
static const Color Cyan = Color(0.0f, 1.0f, 1.0f, 1.0f);
static const Color White = Color(1.0f, 1.0f, 1.0f, 1.0f);
