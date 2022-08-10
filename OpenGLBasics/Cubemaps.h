#pragma once
#include "glPCH.h"
#include "Shader.h"
#include "Graphic.h"

struct CubemapData
{
	CubemapData(std::vector<DVector3> pos, std::vector<uint16_t> _indices)
		:vertPositions(pos), indices(_indices) {}
	std::vector<DVector3> vertPositions;
	std::vector<uint16_t> indices;
	GLuint vao = 0, vbo = 0, ebo = 0, cubemapID = 0;
	
};

static CubemapData defaultSkyBox = CubemapData(
	// verts positions
	{
		DVector3(1.0f, 1.0f, 1.0f),		// 0
		DVector3(1.0f, 1.0f, -1.0f),	// 1
		DVector3(1.0f, -1.0f, 1.0f),	// 2
		DVector3(1.0f, -1.0f, -1.0f),	// 3
		DVector3(-1.0f, 1.0f, 1.0f),	// 4
		DVector3(-1.0f, 1.0f, -1.0f),	// 5
		DVector3(-1.0f, -1.0f, 1.0f),	// 6
		DVector3(-1.0f, -1.0f, -1.0f)	// 7
	}, 
	// indices
	{
		5, 7, 3,
		3, 1, 5,
		6, 7, 5,
		5, 4, 6,
		3, 2, 0,
		0, 1, 3,
		6, 4, 0,
		0, 2, 6,
		5, 1, 0,
		0, 4, 5,
		7, 6, 3,
		3, 6, 2
	});

static float skyboxVertices[108] = {
	// positions          
	-1.0f,  1.0f, -1.0f, // 5
	-1.0f, -1.0f, -1.0f, // 7
	 1.0f, -1.0f, -1.0f, // 3
	 1.0f, -1.0f, -1.0f, // 3
	 1.0f,  1.0f, -1.0f, // 1
	-1.0f,  1.0f, -1.0f, // 5

	-1.0f, -1.0f,  1.0f, // 6 
	-1.0f, -1.0f, -1.0f, // 7
	-1.0f,  1.0f, -1.0f, // 5
	-1.0f,  1.0f, -1.0f, // 5
	-1.0f,  1.0f,  1.0f, // 4
	-1.0f, -1.0f,  1.0f, // 6

	 1.0f, -1.0f, -1.0f, // 3
	 1.0f, -1.0f,  1.0f, // 2
	 1.0f,  1.0f,  1.0f, // 0
	 1.0f,  1.0f,  1.0f, // 0
	 1.0f,  1.0f, -1.0f, // 1
	 1.0f, -1.0f, -1.0f, // 3

	-1.0f, -1.0f,  1.0f, // 6
	-1.0f,  1.0f,  1.0f, // 4
	 1.0f,  1.0f,  1.0f, // 0
	 1.0f,  1.0f,  1.0f, // 0
	 1.0f, -1.0f,  1.0f, // 2
	-1.0f, -1.0f,  1.0f, // 6

	-1.0f,  1.0f, -1.0f, // 5
	 1.0f,  1.0f, -1.0f, // 1
	 1.0f,  1.0f,  1.0f, // 0
	 1.0f,  1.0f,  1.0f, // 0
	-1.0f,  1.0f,  1.0f, // 4
	-1.0f,  1.0f, -1.0f, // 5

	-1.0f, -1.0f, -1.0f, // 7
	-1.0f, -1.0f,  1.0f, // 6
	 1.0f, -1.0f, -1.0f, // 3
	 1.0f, -1.0f, -1.0f, // 3
	-1.0f, -1.0f,  1.0f, // 6
	 1.0f, -1.0f,  1.0f  // 2
};

class Material;

class Cubemaps:
	public Graphic
{
public:
	Cubemaps(std::vector<std::string> files);
	~Cubemaps();
	bool IsValid();
	void AddToBuffer();
	virtual void Draw(DMat4x4 projection, DMat4x4 view) override;
	void SetMaterial(Material* mat);
	void GetShaderFromPath(const char* vsPath, const char* fsPath);
	virtual bool operator==(const Graphic& other);

private:
	Material* skyboxMat;
	bool valid = false;
	GLuint vao, vbo, cubemapID;
	std::vector<void*>textures;
};

