#pragma once
#include "../3DTypeDefs.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
struct TriangleIndices
{
	TriangleIndices(uint32_t index1, uint32_t index2, uint32_t index3)
	{
		indices[0] = index1;
		indices[1] = index2; 
		indices[2] = index3;
	}

	uint32_t indices[3];
};
class PrimitivesLibrary
{
public:
	static void GetSphere(const float& radius, std::vector<vector3>& vertices);
	static void GetIcosahedron(const float& radius, const int& recursionLevel, std::vector<vector3>& vertices, std::vector<uint32_t>& indices);
	static void GetIcosahedron(const float& radius, const int& recursionLevel, std::vector<vector3>& vertices, std::vector<TriangleIndices>& faceIndices, std::vector<uint32_t>& indices);
	static void ComputeNormals(const std::vector<TriangleIndices>& faces, const std::vector<vector3>& points, std::vector<vector3>& normals);
	static void ComputeNormals(const std::vector<uint32_t>& indices, const std::vector<vector3>& vertPoints, std::vector<vector3>& normals);
	static void GetIcosphere(const float& radius, std::vector<vector3>& vertices);
	static void GetCube(const float& radius, std::vector<vector3>& vertices);

private:
	static uint32_t GetMiddlePoint(int p1, int p2,  std::vector<vector3>& points);
};

inline void PrimitivesLibrary::GetIcosahedron(const float& radius, const int& recursionLevel, std::vector<vector3>& vertices, std::vector<uint32_t>& indices)
{
	float tertiaryPoint = (1.0f + glm::sqrt(5.0f) / 2.0f);

	// Create the 12 Unique points of the icosahedron
	std::vector<vector3> uniquePoints;
	uniquePoints.push_back(vector3(-1.0f, tertiaryPoint, 0.0f));
	uniquePoints.push_back(vector3(1.0f, tertiaryPoint, 0.0f));
	uniquePoints.push_back(vector3(-1.0f, -tertiaryPoint, 0.0f));
	uniquePoints.push_back(vector3(1.0f, -tertiaryPoint, 0.0f));

	uniquePoints.push_back(vector3(0.0f, -1.0f, tertiaryPoint));
	uniquePoints.push_back(vector3(0.0f, 1.0f, tertiaryPoint));
	uniquePoints.push_back(vector3(0.0f, -1.0f, -tertiaryPoint));
	uniquePoints.push_back(vector3(0.0f, 1.0f, -tertiaryPoint));

	uniquePoints.push_back(vector3(tertiaryPoint, 0.0f, -1.0f));
	uniquePoints.push_back(vector3(tertiaryPoint, 0.0f, 1.0f));
	uniquePoints.push_back(vector3(-tertiaryPoint, 0.0f, -1.0f));
	uniquePoints.push_back(vector3(-tertiaryPoint, 0.0f, 1.0f));

	// Create the indices for the  20 triangles that will make the icosahedron
	std::vector<TriangleIndices> faces;
	
	// 5 faces around point 0
	faces.push_back(TriangleIndices(0, 11, 5));
	faces.push_back(TriangleIndices(0, 5, 1));
	faces.push_back(TriangleIndices(0, 1, 7));
	faces.push_back(TriangleIndices(0, 7, 10));
	faces.push_back(TriangleIndices(0, 10, 11));

	// 5 adjacent faces
	faces.push_back(TriangleIndices(1, 5, 9));
	faces.push_back(TriangleIndices(5, 11, 4));
	faces.push_back(TriangleIndices(11, 10, 2));
	faces.push_back(TriangleIndices(10, 7, 6));
	faces.push_back(TriangleIndices(7, 1, 8));

	// 5 faces around point 3
	faces.push_back(TriangleIndices(3, 9, 4));
	faces.push_back(TriangleIndices(3, 4, 2));
	faces.push_back(TriangleIndices(3, 2, 6));
	faces.push_back(TriangleIndices(3, 6, 8));
	faces.push_back(TriangleIndices(3, 8, 9));
	
	// 5 adjacent faces
	faces.push_back(TriangleIndices(4, 9, 5));
	faces.push_back(TriangleIndices(2, 4, 11));
	faces.push_back(TriangleIndices(6, 2, 10));
	faces.push_back(TriangleIndices(8, 6, 7));
	faces.push_back(TriangleIndices(9, 8, 1));

	
	// refine triangles
	for (int i = 0; i < recursionLevel; i++)
	{
		std::vector<TriangleIndices> facesTemp;

		for (TriangleIndices tri : faces)
		{
			// replace triangle by 4 triangle
			uint32_t a = GetMiddlePoint(tri.indices[0], tri.indices[1], uniquePoints);
			uint32_t b = GetMiddlePoint(tri.indices[1], tri.indices[2], uniquePoints);
			uint32_t c = GetMiddlePoint(tri.indices[2], tri.indices[0], uniquePoints);

			facesTemp.push_back(TriangleIndices(tri.indices[0], a, c));
			facesTemp.push_back(TriangleIndices(tri.indices[1], b, a));
			facesTemp.push_back(TriangleIndices(tri.indices[2], c, b));
			facesTemp.push_back(TriangleIndices(a, b, c));
		}

		faces = facesTemp;
	}

	vertices = uniquePoints;
	
	for (TriangleIndices tri : faces)
	{
		uint32_t index1 = tri.indices[0];
		uint32_t index2 = tri.indices[1];
		uint32_t index3 = tri.indices[2];

		indices.push_back(index1);
		indices.push_back(index2);
		indices.push_back(index3);
	}
}

inline void PrimitivesLibrary::GetIcosahedron(const float& radius, const int& recursionLevel, std::vector<vector3>& vertices, std::vector<TriangleIndices>& faceIndices, std::vector<uint32_t>& indices)
{
	float tertiaryPoint = (1.0f + glm::sqrt(5.0f) / 2.0f);

	// Create the 12 Unique points of the icosahedron
	std::vector<vector3> uniquePoints;
	uniquePoints.push_back(vector3(-1.0f, tertiaryPoint, 0.0f));
	uniquePoints.push_back(vector3(1.0f, tertiaryPoint, 0.0f));
	uniquePoints.push_back(vector3(-1.0f, -tertiaryPoint, 0.0f));
	uniquePoints.push_back(vector3(1.0f, -tertiaryPoint, 0.0f));

	uniquePoints.push_back(vector3(0.0f, -1.0f, tertiaryPoint));
	uniquePoints.push_back(vector3(0.0f, 1.0f, tertiaryPoint));
	uniquePoints.push_back(vector3(0.0f, -1.0f, -tertiaryPoint));
	uniquePoints.push_back(vector3(0.0f, 1.0f, -tertiaryPoint));

	uniquePoints.push_back(vector3(tertiaryPoint, 0.0f, -1.0f));
	uniquePoints.push_back(vector3(tertiaryPoint, 0.0f, 1.0f));
	uniquePoints.push_back(vector3(-tertiaryPoint, 0.0f, -1.0f));
	uniquePoints.push_back(vector3(-tertiaryPoint, 0.0f, 1.0f));

	// Create the indices for the  20 triangles that will make the icosahedron
	std::vector<TriangleIndices> faces;

	// 5 faces around point 0
	faces.push_back(TriangleIndices(0, 11, 5));
	faces.push_back(TriangleIndices(0, 5, 1));
	faces.push_back(TriangleIndices(0, 1, 7));
	faces.push_back(TriangleIndices(0, 7, 10));
	faces.push_back(TriangleIndices(0, 10, 11));

	// 5 adjacent faces
	faces.push_back(TriangleIndices(1, 5, 9));
	faces.push_back(TriangleIndices(5, 11, 4));
	faces.push_back(TriangleIndices(11, 10, 2));
	faces.push_back(TriangleIndices(10, 7, 6));
	faces.push_back(TriangleIndices(7, 1, 8));

	// 5 faces around point 3
	faces.push_back(TriangleIndices(3, 9, 4));
	faces.push_back(TriangleIndices(3, 4, 2));
	faces.push_back(TriangleIndices(3, 2, 6));
	faces.push_back(TriangleIndices(3, 6, 8));
	faces.push_back(TriangleIndices(3, 8, 9));

	// 5 adjacent faces
	faces.push_back(TriangleIndices(4, 9, 5));
	faces.push_back(TriangleIndices(2, 4, 11));
	faces.push_back(TriangleIndices(6, 2, 10));
	faces.push_back(TriangleIndices(8, 6, 7));
	faces.push_back(TriangleIndices(9, 8, 1));


	// refine triangles
	for (int i = 0; i < recursionLevel; i++)
	{
		std::vector<TriangleIndices> facesTemp;

		for (TriangleIndices tri : faces)
		{
			// replace triangle by 4 triangle
			uint32_t a = GetMiddlePoint(tri.indices[0], tri.indices[1], uniquePoints);
			uint32_t b = GetMiddlePoint(tri.indices[1], tri.indices[2], uniquePoints);
			uint32_t c = GetMiddlePoint(tri.indices[2], tri.indices[0], uniquePoints);

			facesTemp.push_back(TriangleIndices(tri.indices[0], a, c));
			facesTemp.push_back(TriangleIndices(tri.indices[1], b, a));
			facesTemp.push_back(TriangleIndices(tri.indices[2], c, b));
			facesTemp.push_back(TriangleIndices(a, b, c));
		}

		faces = facesTemp;
	}

	vertices = uniquePoints;

	for (TriangleIndices tri : faces)
	{
		uint32_t index1 = tri.indices[0];
		uint32_t index2 = tri.indices[1];
		uint32_t index3 = tri.indices[2];

		indices.push_back(index1);
		indices.push_back(index2);
		indices.push_back(index3);
	}

	faceIndices = faces;
}

inline void PrimitivesLibrary::ComputeNormals(const std::vector<TriangleIndices>& faces, const std::vector<vector3>& points, std::vector<vector3>& normals)
{
	for (TriangleIndices tri : faces)
	{
		// Get two of the edges that form the triangle
		vector3 edge1 = points[tri.indices[0]] - points[tri.indices[1]];
		vector3 edge2 = points[tri.indices[0]] - points[tri.indices[2]];

		// Calculate the normal by getting the direction of the cross product of the two edges
		vector3 normal =  glm::normalize(glm::cross(edge1, edge2));

		// Add it to the normals collection
		// (pushing the normal 3 times to have the normal represent each vertex for the given triangle)
		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}
}

inline void PrimitivesLibrary::ComputeNormals(const std::vector<uint32_t>& indices, const std::vector<vector3>& vertPoints, std::vector<vector3>& normals)
{
	//// loop through three vertex points every iteration
	//for (int i = 2; i < indices.size(); i += 3)
	//{
	//	// Get two of the edges that form this triangle
	//	int index0 = indices[i - 2];
	//	int index1 = indices[i - 1];
	//	int index2 = indices[i];

	//	vector3 edge1 = vertPoints[index1] - vertPoints[index0];
	//	vector3 edge2 = vertPoints[index2] - vertPoints[index0];
	//	// Calculate the normal by getting the direction of the cross product of the two edges
	//	vector3 normal = glm::normalize(glm::cross(edge1, edge2));
	//	// Add it to the normals collection
	//	// (pushing the normal 3x to have the normal represent each vertex for the given triangle)
	//	normals.push_back(normal);
	//	normals.push_back(normal);
	//	normals.push_back(normal);
	//}

	uint32_t index = 0;
	uint32_t offsetIndex = 0;

	// While not at the the end of the vertex position collection
	while (index < (uint32_t)vertPoints.size())
	{
		// Look for next instance of vertex
		auto result = std::find(indices.begin() + offsetIndex, indices.end(), index);
		vector3 normal = vector3(0);
		while(result != indices.end())
		{
			int triIndex1 = 0, triIndex2 = 0, triIndex3 = 0;
			int distanceFromStart = (int)std::distance(indices.begin(), result);
			// if the index is the first vertex in the tri
			if (distanceFromStart % 3 == 0)
			{
				triIndex1 = index;
				// Get the other two indices from the
				// next two points in the collection
				triIndex2 = indices[distanceFromStart + 1];
				triIndex3 = indices[distanceFromStart + 2];

			}
			// if the index is the second vertex in the tri
			else if (distanceFromStart % 3 == 1)
			{
				triIndex2 = index;
				// Get the other two indices from the
				// point before and the point after
				triIndex1 = indices[distanceFromStart - 1];
				triIndex3 = indices[distanceFromStart + 1];
			}
			// if the index is the third vertex in the tri
			else
			{
				triIndex3 = index;
				// Get the other two indices from the
				// previous two points in the collection
				triIndex1 = indices[distanceFromStart - 2];
				triIndex2 = indices[distanceFromStart - 1];
			}
			// calculate the normal for the associated triangle
			vector3 edge1 = vertPoints[triIndex2] - vertPoints[triIndex1];
			vector3 edge2 = vertPoints[triIndex3] - vertPoints[triIndex1];

			vector3 triNormal = glm::cross(edge1, edge2);

			// add the calculated normal to the average for this vertex
			normal += triNormal;

			// attempt to find the next instance of the current index in the collection
			offsetIndex = distanceFromStart + 1;
			result = std::find(indices.begin() + offsetIndex, indices.end(), index);
		}

		// normalize the final value of the normal
		normal = glm::normalize(normal);
		// add it to the collection
		normals.push_back(normal);
		// go to the next vertex
		index++;
		// reset the offset
		offsetIndex = 0;
	}
}

inline uint32_t PrimitivesLibrary::GetMiddlePoint(int p1, int p2, std::vector<vector3>& points)
{
	
	// check if cache has the midpoint already
	vector3 point1 = points[p1];
	vector3 point2 = points[p2];
	vector3 midPoint = (point1 + point2) / 2.0f;

	for (std::vector<vector3>::iterator it = points.begin(); it != points.end(); it++)
		// if so return it's current index in the collection
		if (*it == midPoint)
			return (uint32_t)std::distance(points.begin(), it);
		
	

	// Otherwise add it to the unique point queue
	points.push_back(midPoint);
	// then return it's new index
	uint32_t index = (uint32_t)points.size() - 1;
	return index;
}
