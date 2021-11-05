#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Graphic.h"
#include "Light.h"
#include "Primitives/PrimitivesLibrary.h"
class Mesh: public Graphic
{
public:
	Mesh() { type = typeid(this).name();  };
	Mesh(std::vector<vertex> _vertexPositions, std::vector<uint32_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertexPositions.size();
		indexSize = (uint32_t)indices.size();

		verts = std::move(_vertexPositions);
		indexBuf = std::move(indices);


		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}


	Mesh(std::vector<vector3> _vertexPositions, std::vector<vector3> _normals, std::vector<uint32_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertexPositions.size();
		indexSize = (uint32_t)indices.size();

		std::vector<vector3> positions = std::move(_vertexPositions);
		std::vector<vector3> normals = std::move(_normals);
		indexBuf = std::move(indices);


		// Create a vertex buffer to be passed in the vbo for the mesh
		CreateVertexBuffer(&positions[0], &normals[0]);

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<vector3> _vertexPositions, std::vector<vector2> _uvs, std::vector<vector3> _normals, std::vector<uint32_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertexPositions.size();
		indexSize = (uint32_t)indices.size();

		std::vector<vector3> positions = std::move(_vertexPositions);
		std::vector<vector2> texCoords = std::move(_uvs);
		std::vector<vector3> normals = std::move(_normals);
		indexBuf = std::move(indices);


		// Create a vertex buffer to be passed in the vbo for the mesh
		CreateVertexBuffer(&positions[0], &normals[0], &texCoords[0]);

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}



	Mesh(std::vector<vertex> _vertexPositions,  std::vector<uint32_t> indices, const char* vertexPath, const char* fragmentPath) : Mesh(_vertexPositions, indices)
	{
		SetMultipass(true);
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	Mesh(std::vector<vector3> _vertexPositions, std::vector<vector3> _normals, std::vector<uint32_t> indices, const char* vertexPath, const char* fragmentPath) : Mesh(_vertexPositions, _normals, indices)
	{
		SetMultipass(true);
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}


	Mesh(std::vector<vector3> _vertexPositions, std::vector<vector2> _uvs, std::vector<vector3> _normals, std::vector<uint32_t> indices, const char* vertexPath, const char* fragmentPath): Mesh(_vertexPositions, _uvs, _normals, indices)
	{
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	~Mesh()
	{		
		if (shader)
			delete shader;

		// make sure the vertex info is no longer being passed into glsl
		glDisableVertexAttribArray(0 | 1 | 2 | 3);

		// Deallocate the buffers generated for the GPU
		glDeleteBuffers(1, &vbo); // vertex buffer
		glDeleteBuffers(1, &ebo); // index buffer 
		glDeleteVertexArrays(1, &vao); // vertex array buffer
	}
	
	// BASE CLASS FUNCTIONS
	void CreateVertexBuffer(const vector3* positions, const vector3* normals, const vector2* uvs = nullptr);
	void AddDataToGLBuffer();
	
	virtual void Draw(mat4 projetion, mat4 view) override;
	void ApplyTexture(Texture* texture);
	void GetPivot(vector3* pivotPoint);
	void GetShaderFromPath(const char* vertexPath, const char* fragmentPath);
	virtual bool operator ==(const Graphic& other) override;
	void SetMateialColor(const MLinearColor& color);

	//// TEMPORARY FUNCTIONS
	//void ChangeColors();

	// vertex  buffer
	std::vector<vertex> verts;	
	std::vector<uint32_t> indexBuf;

	// buffer lengths
	uint32_t vertSize = 0;
	uint32_t indexSize = 0;

	// buffer ids
	GLuint vao = 0, vbo = 0, ebo = 0;

	// Shader for mesh
	Shader* shader = nullptr;

	// PRIVATE FUNCTIONS
	private:
		/// <summary>
		/// Function that allows the mesh component to update the mesh's
		/// model matrix (aka transform) before drawing
		/// </summary>
		/// <param name="model"></param>
		void UpdateModelMatrixPos(const mat4& _model);

	private:
	MLinearColor materialColor = Yellow;
	mat4  model = mat4(1.0f);
	uint8_t textureApplied = false;
	GLuint textureID;
	std::vector<Light> lightInfo;
	friend class MeshComponent;
};

