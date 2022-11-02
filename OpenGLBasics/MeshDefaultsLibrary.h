#pragma once
#include "glPCH.h"
class MeshDefaultsLibrary
{
public:
	static void GetCubePrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices);
	static void GetIcosahedronPrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices);
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

inline void MeshDefaultsLibrary::GetIcosahedronPrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices)
{
	const float X	= 0.525731112119133606f;
	const float Z	= 0.850650808352039932f;
	const float Not = 0.0f;
	std::vector<DVector3> vertPositions =
	{
		DVector3(-X, Not,  Z),
		DVector3( X, Not,  Z),
		DVector3(-X, Not, -Z),
		DVector3( X, Not, -Z),
		
		DVector3(Not,  Z,  X),
		DVector3(Not,  Z, -X),
		DVector3(Not, -Z,  X),
		DVector3(Not, -Z, -X),

		DVector3( Z,  X,  Not),
		DVector3(-Z,  X,  Not),
		DVector3( Z, -X,  Not),
		DVector3(-Z, -X,  Not)
	};

	uint16_t triIndices[] =
	{
		0,4,1,
		0,9,4,
		9,5,4,
		4,5,8,
		4,8,1,

		8,10,1,
		8,3,10,
		5,3,8,
		5,2,3,
		2,7,3,

		7,10,3,
		7,6,10,
		7,11,6,
		11,0,6,
		0,1,6,

		6,1,10,
		9,0,11,
		9,11,2,
		9,2,5,
		7,2,11
	};

	uint16_t triCount = sizeof(triIndices) / sizeof(uint16_t) / 3;
	vertices.clear();
	indices.clear();
	for (int i = 0; i < triCount; i++)
	{
		uint16_t firstIndex  = triIndices[i * 3];
		uint16_t secondIndex = triIndices[i * 3 + 1];
		uint16_t thirdIndex  = triIndices[i * 3 + 2];

		DVector3 firstPos	= vertPositions[firstIndex];
		DVector3 secondPos	= vertPositions[secondIndex];
		DVector3 thirdPos	= vertPositions[thirdIndex];
		// Calculate the normal
		DVector3 edge1 = secondPos - firstPos;
		DVector3 edge2 = thirdPos  - firstPos;
		DVector3 normal = glm::normalize(glm::cross(edge1, edge2));
		// Make sure the normal is facing outward
		if (glm::dot(normal, firstPos) < 0.0f ||
			glm::dot(normal, secondPos) < 0.0f ||
			glm::dot(normal, thirdPos) < 0.0f)
			normal *= -1;
		for (int j = 0; j < 3; j++)
		{
			DVertex tempVertex;
			uint16_t index;
			if (j == 0)
				tempVertex = DVertex(firstPos, normal);
			else if (j == 1)
				tempVertex = DVertex(secondPos, normal);
			else
				tempVertex = DVertex(thirdPos, normal);
			
			auto search = std::find(vertices.begin(), vertices.end(), tempVertex);
			if (search == vertices.end())
			{
				index = (uint16_t)vertices.size();
				vertices.push_back(tempVertex);
			}
			else
				index = (uint16_t)std::distance(vertices.begin(), search);
			indices.push_back((uint16_t)index);
		}
		
	}	
}



inline void MeshDefaultsLibrary::GetSpherePrimitive(int subDivideCount, std::vector<DVector3>& vertices, std::vector<uint16_t>& indices)
{
	// Create Icosahedron
	// Start subdivisions
}