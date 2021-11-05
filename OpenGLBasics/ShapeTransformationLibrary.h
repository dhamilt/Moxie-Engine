#pragma once
#include "glPCH.h"
class ShapeTransformationLibrary
{
public:
	/// <summary>
	/// Returns the points in the format of a sphere
	/// </summary>
	/// <param name="longitude">The number of lines of longitude</param>
	/// <param name="lattitude">The number of lines of lattitude</param>
	/// <param name="radius">The radius of of the sphere</param>
	/// <param name="origin">The origin point of the sphere</param>
	/// <param name="points">The collection of sphere points being returned</param>
	static void GetSpherePoints(int longitude, int lattitude, float radius, vector3 origin, std::vector<vector3>& points);
	/// <summary>
	/// Draws a Maurer rose based on the number of legs and petals
	/// Keep in mind, if the number of petals is even the rose will
	/// have twice that value
	/// </summary>
	/// <param name="numOfPetals">the number of petals in the rose</param>
	/// <param name="degreesBetween">The angle of the petal</param>
	/// <param name="width">The width of the rose</param>
	/// <param name="height">The height of the rose</param>
	/// <param name="origin">The origin point of the rose</param>
	/// <param name="points">The points of the rose</param>
	static void GetMaurerRosePoints(unsigned int numOfPetals, float degreesBetweeen, float width, float height, vector3 origin, std::vector<vector3>& points);

};

inline void ShapeTransformationLibrary::GetSpherePoints(int longitude, int lattitude, float radius, vector3 origin, std::vector<vector3>& points)
{
	if (!points.empty())
		points.clear();

	float twoPI = glm::pi<float>() * 2;

	for (int i = 0; i < lattitude; i++)
	{
		for (int j = 0; j < longitude; j++)
		{
			float x, y, z;

			x = glm::cos((float)i / (float)lattitude * twoPI) * glm::sin((float)j / (float)longitude * twoPI) * radius;
			y = glm::sin((float)i / (float)lattitude * twoPI) * glm::sin((float)j / (float)longitude * twoPI) * radius;
			z = glm::sin((float)j / (float)longitude * twoPI) * radius;

			vector3 point = vector3(x, y, z);
			point += origin;

			points.push_back(point);
		}
	}
}

inline void ShapeTransformationLibrary::GetMaurerRosePoints(unsigned int numOfPetals, float degreesBetween, float width, float height, vector3 origin, std::vector<vector3>& points)
{
	// clear vector points collection
	if (points.size() > 0)
		points.clear();

	// add the origin point as the starting point
	points.push_back(origin);
		
	for (int theta = 0; theta <= 360; theta++)
	{
		float k = theta * degreesBetween /** glm::pi<float>() / 180.0f*/;
		float r = 300 * glm::sin(numOfPetals * k);
		float x = r * glm::cos(k) * width;
		float y = r * glm::sin(k) * height;
		

		vector3 result = vector3(x, y, 0);
		result += origin;
		points.push_back(result);
	}

	// add the origin point as the ending point
	points.push_back(origin);
	
}
