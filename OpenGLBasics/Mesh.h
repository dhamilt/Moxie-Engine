#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Graphic.h"
#include "Light.h"
class Mesh: public Graphic
{
public:
	Mesh() { type = typeid(this).name(); };
	Mesh(std::vector<vertex> _vertices, std::vector<uint16_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertices.size();
		indexSize = (uint32_t)indices.size();

		verts = new vertex[vertSize];
		indexBuf = new uint16_t[indexSize];

		memcpy(verts, &_vertices[0], sizeof(vertex) * vertSize);
		memcpy(indexBuf, &indices[0], sizeof(uint16_t) * indexSize);

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<vector3> _vertices, std::vector<vector3> _normals, std::vector<uint16_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertices.size();
		indexSize = (uint32_t)indices.size();

		positions = new vector3[vertSize];
		normals = new vector3[vertSize];
		indexBuf = new uint16_t[indexSize];

		memcpy(positions, &_vertices[0], sizeof(vector3) * _vertices.size());
		memcpy(normals, &_normals[0], sizeof(vector3) * _normals.size());
		memcpy(indexBuf, &indices[0], sizeof(uint16_t) * indices.size());

		// Create a vertex buffer to be passed in the vbo for the mesh
		CreateVertexBuffer();

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<vector3> _vertices, std::vector<vector2> _uvs, std::vector<vector3> _normals, std::vector<uint16_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertices.size();
		indexSize = (uint32_t)indices.size();

		positions = new vector3[vertSize];
		texCoords = new vector2[vertSize];
		normals = new vector3[vertSize];
		indexBuf = new uint16_t[indexSize];

		memcpy(positions, &_vertices[0], sizeof(vector3) * _vertices.size());
		memcpy(texCoords, &_uvs[0], sizeof(vector2) * _uvs.size());
		memcpy(normals, &_normals[0], sizeof(vector3) * _normals.size());
		memcpy(indexBuf, &indices[0], sizeof(uint16_t) * indices.size());

		// Create a vertex buffer to be passed in the vbo for the mesh
		CreateVertexBuffer();

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<vertex> _vertices,  std::vector<uint16_t> indices, const char* vertexPath, const char* fragmentPath) : Mesh(_vertices, indices)
	{
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	Mesh(std::vector<vector3> _vertices, std::vector<vector3> _normals, std::vector<uint16_t> indices, const char* vertexPath, const char* fragmentPath) : Mesh(_vertices, _normals, indices)
	{
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	Mesh(std::vector<vector3> _vertices, std::vector<vector2> _uvs, std::vector<vector3> _normals, std::vector<uint16_t> indices, const char* vertexPath, const char* fragmentPath): Mesh(_vertices, _uvs, _normals, indices)
	{
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	~Mesh()
	{
		if (verts)
			delete[] verts;
		if (positions)
			delete[] positions;
		if (texCoords)
			delete[] texCoords;
		if (normals)
			delete[] normals;
		if (indexBuf)
			delete[] indexBuf;
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
	void CreateVertexBuffer();
	void AddDataToGLBuffer();
	
	virtual void Draw(mat4 projetion, mat4 view) override;
	void ApplyTexture(Texture* texture);
	void GetPivot(vector3* pivotPoint);
	void GetShaderFromPath(const char* vertexPath, const char* fragmentPath);
	virtual bool operator ==(const Graphic& other) override;

	//// TEMPORARY FUNCTIONS
	//void ChangeColors();

	// vertex  buffer
	vertex* verts = nullptr;
	vector3* positions  = nullptr;
	vector3* normals =  nullptr;
	vector2* texCoords = nullptr;
	uint16_t* indexBuf = nullptr;

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
	Color materialColor = Yellow;
	mat4  model = mat4(1.0f);
	uint8_t textureApplied = false;
	GLuint textureID;
	std::vector<Light> lightInfo;
	friend class MeshComponent;
};

