#pragma once
#include "Color.h"
// struct representing the light info that will passed into glsl
struct Light
{
	int type;
	DVector3 position;
	DVector3 direction;
	float radius = 5;

	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;

	DVector3 ambient = DVector3(0.2f, 0.2f, 0.2f);
	DVector3 diffuse = DVector3(1.0f, 1.0f, 0.0f);
	DVector3 specular = DVector3(1.0f, 0.5f, 0.0f);

	float lightIntensity = 1.0f;
	Color lightColor = White;
};

