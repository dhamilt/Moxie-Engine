#pragma once
#include "glPCH.h"
#include <unordered_map>
class MeshDefaultsLibrary
{
public:
	static void GetCubePrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices);
	static void GetIcosahedronPrimitive(std::vector<DVertex>& vertices, std::vector<uint16_t>& indices);
	static void GetSpherePrimitive(int subDivideCount, std::vector<DVertex>& vertices, std::vector <uint16_t>& indices);
	
	
private:
	static void GetSpherePrimitive(int subDivideCount, int currentSubdivide, std::vector<DVertex>& vertices, std::vector <uint16_t>& indices);
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
	const float X	= 1.0f;
	const float Z	= (1.0f +std::sqrtf(5.0f))/2.0f;
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



inline void MeshDefaultsLibrary::GetSpherePrimitive(int subDivideCount, std::vector<DVertex>& vertices, std::vector<uint16_t>& indices)
{
	// Create Icosahedron	
	MeshDefaultsLibrary::GetIcosahedronPrimitive(vertices, indices);

	// Start subdivisions
	GetSpherePrimitive(subDivideCount, 0, vertices, indices);
}

inline void MeshDefaultsLibrary::GetSpherePrimitive(int subDivideCount, int currentSubdivide, std::vector<DVertex>& vertices, std::vector<uint16_t>& indices)
{
	if (currentSubdivide >= subDivideCount)
		return;
	std::unordered_map<DVertex, uint16_t,  DVertexHash> icoVertsCache;
	std::vector<DVertex> icoVerts;
	std::vector<uint16_t> icoIndices;
	uint32_t triCount = uint32_t(indices.size() / 3);
	uint32_t index = 0;
	// for each triangle
	for (uint32_t i = 0; i < triCount; i++)
	{
		DVertex A = vertices[indices[i * 3]];
		DVertex B = vertices[indices[i * 3 + 1]];
		DVertex C = vertices[indices[i * 3 + 2]];

		// get midpoint for each edge of triangle
		DVector3 midAB = DVectorLibrary::GetMidPoint(A.pos, B.pos);
		DVector3 midBC = DVectorLibrary::GetMidPoint(B.pos, C.pos);
		DVector3 midCA = DVectorLibrary::GetMidPoint(C.pos, A.pos);		

		// make four triangles using each midpoint		
		// 0, midAB, midCA
		// midAB, 1, midBC
		// midCA, midBC, 2
		// midCA, midAB, midBC
		std::vector < std::vector < DVector3 >>triangles= 
		{
			{A.pos, midAB, midCA},
			{midAB, B.pos, midBC},
			{midCA, midBC, C.pos},
			{midBC, midCA, midAB}
		};
		for (uint32_t j = 0; j < 4; j++)
		{
			// Calculate normals for newly subdivided triangles
			DVector3 vA = glm::normalize(triangles[j][0]);
			DVector3 vB = glm::normalize(triangles[j][1]);
			DVector3 vC = glm::normalize(triangles[j][2]);

			DVector3 edge_vAB = vB - vA;
			DVector3 edge_vAC = vC - vA;

			DVector3 normal = glm::normalize(glm::cross(edge_vAB, edge_vAC));
			// orientation check
			if (glm::dot(normal, vA) < 0.0f ||
				glm::dot(normal, vB) < 0.0f ||
				glm::dot(normal, vC) < 0.0f)
				normal *= 1;

			// Create new vertices with the created positions and normals
			for (uint32_t k = 0; k < 3; k++)
			{
				DVertex temp;
				if (k == 0)
					temp = DVertex(vA, normal);
				else if (k == 1)
					temp = DVertex(vB, normal);
				else
					temp = DVertex(vC, normal);
				if (icoVertsCache.find(temp) == icoVertsCache.end())
				{
					icoVerts.push_back(temp);
					icoVertsCache.insert({ temp, index });
					index++;
				}
				icoIndices.push_back(icoVertsCache[temp]);
			}
		}
	}
	
	
	MeshDefaultsLibrary::GetSpherePrimitive(subDivideCount, currentSubdivide + 1, icoVerts, icoIndices);
	vertices = icoVerts;
	indices = icoIndices;
}
