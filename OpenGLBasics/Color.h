#pragma once

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
	static const Color Blue;
	static const Color Green;
	static const Color Red;
	static const Color Yellow;
	static const Color Purple;
	static const Color Cyan;
	static const Color White;
};


inline void Color::ClampValues()
{
	this->r = glm::clamp(this->r, 0.0f, 1.0f);
	this->g = glm::clamp(this->g, 0.0f, 1.0f);
	this->b = glm::clamp(this->b, 0.0f, 1.0f);
	this->a = glm::clamp(this->a, 0.0f, 1.0f);
}

const inline Color Color::Blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
const inline Color Color::Green = Color(0.0f, 1.0f, 0.0f, 1.0f);
const inline Color Color::Red = Color(1.0f, 0.0f, 0.0f, 1.0f);
const inline Color Color::Purple = Color(1.0f, 0.0f, 1.0f, 1.0f);
const inline Color Color::Yellow = Color(1.0f, 1.0f, 0.0f, 1.0f);
const inline Color Color::Cyan = Color(0.0f, 1.0f, 1.0f, 1.0f);
const inline Color Color::White = Color(1.0f, 1.0f, 1.0f, 1.0f);
