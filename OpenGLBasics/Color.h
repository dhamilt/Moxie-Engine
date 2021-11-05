#pragma once
#include "glPCH.h"

struct MLinearColor;

struct MColor
{
	MColor()
		:
		r(0)
	,	g(0)
	,	b(0)
	,	a(0)
	{}

	MColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
		:
		r(_r)
	,	g(_g)
	,	b(_b)
	,	a(_a)
	{
		
	}
	vector2 rg() { return vector2(r, g); };
	vector2 rb() { return vector2(r, b); };
	vector2 gb() { return vector2(g, b); };
	vector3 rgb() { return vector3(r, g, b); };
	vector4 rgba() { return vector4(r, g, b, a); };
	float* ToFloatArray();
	float* ToFloatArray(const MColor& color);
	static MLinearColor ToLinearColor(const MColor& color);

	uint8_t r, g, b, a;
	
	
};

struct MLinearColor
{
	MLinearColor()
		:
		r(0.0f)
		, g(0.0f)
		, b(0.0f)
		, a(0.0f)
	{}

	MLinearColor(float _r, float _g, float _b, float _a)
		:
		r(_r)
		, g(_g)
		, b(_b)
		, a(_a)
	{

	}

	MLinearColor operator*(const float& value) {
		return MLinearColor(this->r * value,
			this->g * value,
			this->b * value,
			this->a * value);
	}

	void operator*=(const float& value) {
		this->r *= value;
		this->g *= value;
		this->b *= value;
		this->a *= value;
	}

	vector2 rg() { return vector2(r, g); };
	vector2 rb() { return vector2(r, b); };
	vector2 gb() { return vector2(g, b); };
	vector3 rgb() { return vector3(r, g, b); };
	vector4 rgba() { return vector4(r, g, b, a); };
	float* ToFloatArray();
	float* ToFloatArray(const MLinearColor& color);
	float r, g, b, a;
};

static const MLinearColor Blue = MLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
static const MLinearColor Green = MLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
static const MLinearColor Red = MLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
static const MLinearColor Purple = MLinearColor(1.0f, 0.0f, 1.0f, 1.0f);
static const MLinearColor Yellow = MLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
static const MLinearColor Cyan = MLinearColor(0.0f, 1.0f, 1.0f, 1.0f);
static const MLinearColor White = MLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
