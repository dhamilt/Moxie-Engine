#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Graphic.h"
#include "Light.h"
#include "MeshData.h"
class Mesh: public Graphic
{
public:
	Mesh() { type = typeid(this).name(); };

	Mesh(std::vector<DVertex> _vertices, std::vector<uint16_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertices.size();
		indexSize = (uint32_t)indices.size();

		verts = new DVertex[vertSize];
		indexBuf = new uint16_t[indexSize];

		memcpy(verts, &_vertices[0], sizeof(DVertex) * vertSize);
		memcpy(indexBuf, &indices[0], sizeof(uint16_t) * indexSize);

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<DVector3> _vertices, std::vector<DVector3> _normals, std::vector<uint16_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertices.size();
		indexSize = (uint32_t)indices.size();

		positions = new DVector3[vertSize];
		normals = new DVector3[vertSize];
		indexBuf = new uint16_t[indexSize];

		memcpy(positions, &_vertices[0], sizeof(DVector3) * _vertices.size());
		memcpy(normals, &_normals[0], sizeof(DVector3) * _normals.size());
		memcpy(indexBuf, &indices[0], sizeof(uint16_t) * indices.size());

		// Create a vertex buffer to be passed in the vbo for the mesh
		CreateVertexBuffer();

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<DVector3> _vertices, std::vector<DVector2> _uvs, std::vector<DVector3> _normals, std::vector<uint16_t> indices)
	{
		type = typeid(this).name();
		vertSize = (uint32_t)_vertices.size();
		indexSize = (uint32_t)indices.size();

		positions = new DVector3[vertSize];
		texCoords = new DVector2[vertSize];
		normals = new DVector3[vertSize];
		indexBuf = new uint16_t[indexSize];

		memcpy(positions, &_vertices[0], sizeof(DVector3) * _vertices.size());
		memcpy(texCoords, &_uvs[0], sizeof(DVector2) * _uvs.size());
		memcpy(normals, &_normals[0], sizeof(DVector3) * _normals.size());
		memcpy(indexBuf, &indices[0], sizeof(uint16_t) * indices.size());

		// Create a vertex buffer to be passed in the vbo for the mesh
		CreateVertexBuffer();

		// pass in the vertex buffer data for opengl to render
		AddDataToGLBuffer();
	}

	Mesh(std::vector<DVertex> _vertices,  std::vector<uint16_t> indices, const char* vertexPath, const char* fragmentPath) 
	{
		meshData = new MeshDataParams(_vertices, indices);
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	Mesh(std::vector<DVector3> _vertices, std::vector<DVector3> _normals, std::vector<uint16_t> indices, const char* vertexPath, const char* fragmentPath) : Mesh(_vertices, _normals, indices)
	{
		type = typeid(this).name();
		shader = new Shader(vertexPath, fragmentPath);
	}

	Mesh(std::vector<DVector3> _vertices, std::vector<DVector2> _uvs, std::vector<DVector3> _normals, std::vector<uint16_t> indices, const char* vertexPath, const char* fragmentPath): Mesh(_vertices, _uvs, _normals, indices)
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
		if (meshData)
			delete meshData;
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
	// Import mesh data
	void Import(std::vector<DVertex> _vertices, std::vector<uint16_t> indices, std::string meshName);
	MeshDataParams* GetMeshData();
	virtual void Draw(DMat4x4 projetion, DMat4x4 view) override;
	void ApplyTexture(Texture* texture);
	void GetPivot(DVector3* pivotPoint);
	void GetShaderFromPath(const char* vertexPath, const char* fragmentPath);
	virtual bool operator ==(const Graphic& other) override;

	//// TEMPORARY FUNCTIONS
	//void ChangeColors();

	// vertex  buffer
	DVertex* verts = nullptr;
	DVector3* positions  = nullptr;
	DVector3* normals =  nullptr;
	DVector2* texCoords = nullptr;
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
		void UpdateModelMatrixPos(const DMat4x4& _model);

	private:
		MeshDataParams* meshData;
		Color materialColor = Yellow;
		DMat4x4  model = DMat4x4(1.0f);
		uint8_t textureApplied = false;
		GLuint textureID;
		std::vector<Light> lightInfo;
		friend class MeshComponent;
};

