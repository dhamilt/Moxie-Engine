#pragma once
#include "Color.h"
// struct representing the light info that will passed into glsl
struct Light
{
	int type;
	vector3 position;
	vector3 direction;
	float radius = 5;

	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;

	vector3 ambient = vector3(0.2f, 0.2f, 0.2f);
	vector3 diffuse = vector3(1.0f, 1.0f, 0.0f);
	vector3 specular = vector3(1.0f, 0.5f, 0.0f);

	float lightIntensity = 1.0f;
	Color lightColor = White;
};

