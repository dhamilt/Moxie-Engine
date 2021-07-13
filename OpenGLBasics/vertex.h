#pragma once
#include "3DTypeDefs.h"



class vertex
{
public:
	vertex() { pos = vector3(0.0f, 0.0f, 0.0f); texCoord = vector2(0.0f, 0.0f); normal = vector3(0.0f, 0.0f, 0.0f); }
	vertex(vector3 _pos, vector2 _texCoord, vector3 _normal) : pos(_pos), texCoord(_texCoord), normal(_normal) {}
	vertex(vector3 _pos, vector3 _normal) : pos(_pos), normal(_normal) { texCoord = vector2(0.0f, 0.0f); }
	vertex(aiVector3D _pos, aiVector2D _texCoord, aiVector3D _normal) { pos = vector3(_pos.x, _pos.y, _pos.z); texCoord = vector2(_texCoord.x, _texCoord.y); normal = vector3(_normal.x, _normal.y, _normal.z); }
	vertex(aiVector3D _pos, aiVector3D _texCoord, aiVector3D _normal) { pos = vector3(_pos.x, _pos.y, _pos.z); texCoord = vector2(_texCoord.x, _texCoord.y); normal = vector3(_normal.x, _normal.y, _normal.z); }
	vertex(aiVector3D _pos, aiVector3D _normal) : texCoord(0, 0) { pos = vector3(_pos.x, _pos.y, _pos.z); normal = vector3(_normal.x, _normal.y, _normal.z); }
	vector3 pos;
	vector2 texCoord;
	vector3 normal;
	vector3 color = vector3();

	inline bool operator==(const vertex& other)
	{
		return pos == other.pos &&
			texCoord == other.texCoord &&
			normal == other.normal;
	}

	inline bool operator!=(const vertex& other)
	{
		return pos != other.pos &&
			texCoord != other.texCoord &&
			normal != other.normal;
	}

};

struct vertex_hash
{
public:

	size_t operator()(const vertex& other) const
	{
		return (std::hash<float>()(other.pos.x) && std::hash<float>()(other.pos.y) && std::hash<float>()(other.pos.z) ^
			std::hash<float>()(other.texCoord.s) && std::hash<float>()(other.texCoord.t) ^
			std::hash<float>()(other.normal.x) && std::hash<float>()(other.normal.y) && std::hash<float>()(other.normal.z));
	}


};

struct vertex_comparator
{
	bool operator()(const vertex& other1, const vertex& other2) const
	{
		return other1.pos == other2.pos &&
			other1.texCoord == other2.texCoord &&
			other1.normal == other2.normal;
	}
};

