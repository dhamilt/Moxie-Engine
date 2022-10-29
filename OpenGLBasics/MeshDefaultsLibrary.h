#pragma once
#include "glPCH.h"
class MeshDefaultsLibrary
{
public:
	static void GetCubePrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices);
	static void GetSpherePrimitive(int subDivideCount, std::vector<DVector3>& vertices, std::vector <uint16_t>& indices);
};

inline void  MeshDefaultsLibrary::GetCubePrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices)
{
	// get all of the unique vertex positions that make a cube
	std::vector<DVector3>vertexPositions = 
	{
		DVector3(1,1,1),
		DVector3(-1,1,1),
		DVector3(1,-1,1),
		DVector3(-1,-1,1),
		DVector3(1,1,-1),
		DVector3(-1,1,-1),
		DVector3(1,-1,-1),
		DVector3(-1,-1,-1)
	};

	// get all of the locations to make triangles from the cube (2 per face)
	uint16_t triIndices[] =
	{
		0,1,2,
		1,2,3,
		0,1,4,
		1,4,5,
		1,5,3,
		5,3,7,
		0,4,2,
		4,2,6,
		2,3,6,
		3,6,7,
		4,5,6,
		5,6,7
	};
	uint32_t triCount = sizeof(triIndices) / sizeof(uint16_t) / 3;
	std::vector<DVertex> uniqueVerts;
	for (uint32_t i = 1; i <= triCount; i++)
	{
		uint32_t triIndex1 = triIndices[(i - 1) * 3];
		uint32_t triIndex2 = triIndices[(i - 1) * 3 + 1];
		uint32_t triIndex3 = triIndices[(i - 1) * 3 + 2];
		DVector3 vertPos1 = vertexPositions[triIndex1];
		DVector3 vertPos2 = vertexPositions[triIndex2];
		DVector3 vertPos3 = vertexPositions[triIndex3];
		// calculate the surface normal for the tri
		DVector3 edge1 = vertPos3 - vertPos1;
		DVector3 edge2 = vertPos2 - vertPos1;
		DVector3 normal = glm::normalize(glm::cross(edge1, edge2));
		// Check to see if normal is inverted
		if (glm::dot(normal, vertPos1) < 0.0 || glm::dot(normal, vertPos2) < 0.0 || glm::dot(normal, vertPos3) < 0.0)
			normal *= -1;
		// Calculate the texture coordinate TODO: Find formula to calculate texture coordinates for mesh

		// create 3 vertices
		for (int j = 0; j < 3; j++)
		{
			DVertex vert;
			if(j == 0)
				vert = DVertex(vertPos1, normal);
			else if(j == 1)
				vert = DVertex(vertPos2, normal);
			else
				 vert= DVertex(vertPos3, normal);
			auto search = std::find(uniqueVerts.begin(), uniqueVerts.end(), vert);
			uint16_t index;
			if (search == uniqueVerts.end())
			{
				index = (uint16_t)uniqueVerts.size();
				uniqueVerts.push_back(vert);
				
				vertices.push_back(vert);
			}
			else
				index = (uint16_t)std::distance(uniqueVerts.begin(), search);
			indices.push_back(index);
		}
	}
}

inline void MeshDefaultsLibrary::GetSpherePrimitive(int subDivideCount, std::vector<DVector3>& vertices, std::vector<uint16_t>& indices)
{
	// Create Icosahedron
	// Start subdivisions
}