#pragma once
#include "glPCH.h"
#include "Texture.h"
#include "vertex.h"
// Data structure for containing Static Mesh Data parameters
struct MeshDataParams
{
	// Force construction with params
	MeshDataParams() = delete;
	std::string name = "defaultMeshName";
	MeshDataParams(std::vector<DVertex> _vertices, std::vector<uint16_t> _indices)
	{
		vertices = _vertices;
		indices = _indices;
	}
	
	MeshDataParams(std::vector<DVector3> positions, std::vector<DVector3> normals, std::vector<uint16_t> _indices)
	{
		assert(positions.size() == normals.size());
		
		int size = 0, index = 0;
		while (index < size)
		{
			DVector3 pos	= positions[index];
			DVector3 normal = normals[index];
			vertices.push_back(DVertex(pos, normal));

			index++;
		}

		indices = _indices;
	}

	MeshDataParams(std::vector<DVector3> positions, std::vector<DVector2> texCoords, std::vector<DVector3> normals, std::vector<uint16_t> indices)
	{
		assert(positions.size() == texCoords.size() == normals.size());

		int size = 0, index = 0;
		while (index < size)
		{
			DVector3 pos = positions[index];
			DVector2 texCoord = texCoords[index];
			DVector3 normal = normals[index];
			vertices.push_back(DVertex(pos, texCoord, normal));
			index++;
		}
	}
	~MeshDataParams()
	{
		if (vertices.size() > 0)
			vertices.clear();
		if (indices.size() > 0)
			indices.clear();
	}
	// Vertex data struct
	std::vector<DVertex> vertices;
	std::vector<uint16_t> indices;
	Texture* texture = nullptr;
	
};