#pragma once
#include "3DTypeDefs.h"



class DVertex
{
public:
	DVertex() { pos = DVector3(0.0f, 0.0f, 0.0f); texCoord = DVector2(0.0f, 0.0f); normal = DVector3(0.0f, 0.0f, 0.0f); }
	DVertex(DVector3 _pos, DVector2 _texCoord, DVector3 _normal) : pos(_pos), texCoord(_texCoord), normal(_normal) {}
	DVertex(DVector3 _pos, DVector3 _normal) : pos(_pos), normal(_normal) { texCoord = DVector2(0.0f, 0.0f); }
	DVertex(aiVector3D _pos, aiVector2D _texCoord, aiVector3D _normal) { pos = DVector3(_pos.x, _pos.y, _pos.z); texCoord = DVector2(_texCoord.x, _texCoord.y); normal = DVector3(_normal.x, _normal.y, _normal.z); }
	DVertex(aiVector3D _pos, aiVector3D _texCoord, aiVector3D _normal) { pos = DVector3(_pos.x, _pos.y, _pos.z); texCoord = DVector2(_texCoord.x, _texCoord.y); normal = DVector3(_normal.x, _normal.y, _normal.z); }
	DVertex(aiVector3D _pos, aiVector3D _normal) : texCoord(0, 0) { pos = DVector3(_pos.x, _pos.y, _pos.z); normal = DVector3(_normal.x, _normal.y, _normal.z); }
	DVector3 pos;
	DVector2 texCoord;
	DVector3 normal;
	DVector3 color = DVector3();

	inline bool operator==(const DVertex& other)
	{
		return pos == other.pos &&
			texCoord == other.texCoord &&
			normal == other.normal;
	}

	inline bool operator!=(const DVertex& other)
	{
		return pos != other.pos &&
			texCoord != other.texCoord &&
			normal != other.normal;
	}

};

struct vertex_hash
{
public:

	size_t operator()(const DVertex& other) const
	{
		return (std::hash<float>()(other.pos.x) & std::hash<float>()(other.pos.y) & std::hash<float>()(other.pos.z) ^
			std::hash<float>()(other.texCoord.s) & std::hash<float>()(other.texCoord.t) ^
			std::hash<float>()(other.normal.x) & std::hash<float>()(other.normal.y) & std::hash<float>()(other.normal.z));
	}


};

struct vertex_comparator
{
	bool operator()(const DVertex& other1, const DVertex& other2) const
	{
		return other1.pos == other2.pos &&
			other1.texCoord == other2.texCoord &&
			other1.normal == other2.normal;
	}
};

