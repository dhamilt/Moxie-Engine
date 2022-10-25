#include "glPCH.h"
#include "Mesh.h"
#include "GLSetup.h"

extern GLSetup* GGLSetup;
void Mesh::CreateVertexBuffer()
{
	verts = new DVertex[vertSize];

	for (uint32_t i = 0; i < vertSize; i++)
	{
		DVertex temp;

		temp.pos = positions[i];

		if (texCoords)
			temp.texCoord = texCoords[i];

		temp.normal = normals[i];

		verts[i] = temp;
	}
}

void Mesh::AddDataToGLBuffer()
{
	// create buffers for the vertex array object, vertex buffer object, and element buffer object
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind the buffers and pass in the appropriate data for the following:
	// the vertex array object
	glBindVertexArray(vao);
	// the vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DVertex) * vertSize, verts, GL_STATIC_DRAW);
	// the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexSize, indexBuf, GL_STATIC_DRAW);

	// Add vertex positions for model
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // start index
		3, // number of dimensions
		GL_FLOAT, // vertex buffer type
		GL_FALSE, // normalized?
		sizeof(DVertex), // stride
		(void*)offsetof(DVertex, DVertex::pos) // buffer offset
	);

	// Add vertex texture coordinates for model	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(DVertex),
		(void*)offsetof(DVertex, DVertex::texCoord)
	);

	// Add vertex normals for model	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // layout index
		3, // number of dimensions
		GL_FLOAT, // value type for dimensions
		GL_FALSE, // normalized?
		sizeof(DVertex), // stride
		(void*)offsetof(DVertex, DVertex::normal)// buffer offset
	);

	//// Add vertex colors for model
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(
	//	3, // layout index
	//	3, // number of dimensions
	//	GL_FLOAT, // value type for dimensions
	//	GL_FALSE, // normalized
	//	sizeof(DVertex), // stride
	//	(void*)offsetof(DVertex, DVertex::color) // buffer offset
	//);
}

void Mesh::Import(std::vector<DVertex> _vertices, std::vector<uint16_t> indices, std::string meshName)
{
	if (!meshData)
		meshData = new MeshDataParams(_vertices, indices);
	else
	{
		meshData->vertices	= _vertices;
		meshData->indices	= indices;
	}
	meshData->name = meshName;
}

MeshDataParams* Mesh::GetMeshData()
{
	return meshData;
}

void Mesh::Draw(DMat4x4 projection, DMat4x4 view)
{
	// retreive the up to date light info
	GGLSPtr->GetAllLightInfo(lightInfo);
	if (textureApplied)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (shader)
	{
		
		shader->Use();
		shader->SetMat4("view", view);
		shader->SetMat4("projection", projection);
		shader->SetMat4("model", model);
		shader->SetFloat3("viewPos", GGLSPtr->mainCamera->transform.GetPosition());
		shader->SetFloat3("objectColor", materialColor.rgb());
		
		// add all of the light info to be drawn on the mesh
		for (int i = 0; i < lightInfo.size(); i++)
		{
			Light* light = &lightInfo[i];
			std::string attribName;
			std::string index = "[" + std::to_string(i) +"]";
			
			attribName = "lights" + index + ".type";
			shader->SetInt(attribName.c_str(), light->type);

			attribName = "lights" + index + ".position";
			shader->SetFloat3(attribName.c_str(), light->position);

			attribName = "lights" + index + ".direction";
			shader->SetFloat3(attribName.c_str(), light->direction);

			attribName = "lights" + index + ".constant";
			shader->SetFloat(attribName.c_str(), light->constant);

			attribName = "lights" + index + ".linear";
			shader->SetFloat(attribName.c_str(), light->linear);

			attribName = "lights" + index + ".quadratic";
			shader->SetFloat(attribName.c_str(), light->quadratic);

			attribName = "lights" + index + ".lightPower";
			shader->SetFloat(attribName.c_str(), light->lightIntensity);

			attribName = "lights" + index + ".lightColor";
			shader->SetFloat3(attribName.c_str(), light->lightColor.rgb());

			attribName = "lights" + index + "._ambient";
			shader->SetFloat3(attribName.c_str(), light->ambient);

			attribName = "lights" + index + "._diffuse";
			shader->SetFloat3(attribName.c_str(), light->diffuse);

			attribName = "lights" + index + "._specular";
			shader->SetFloat3(attribName.c_str(), light->specular);

			attribName = "lights" + index + ".radius";
			shader->SetFloat(attribName.c_str(), light->radius);
		}

		// Pass in the number of lights there are in the scene
		shader->SetInt("lightSize", (int)lightInfo.size());
	}
	glBindVertexArray(vao);	
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::ApplyTexture(Texture* texture)
{
	if (texture->IsValid())
	{
		// generate space on GPU for texture		
		glGenTextures(1, &textureID);
		// then bind it to the identifier
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		// Retrieve the texture parameter's attributes
		int width, height, channels;
		texture->GetAttributes(&width, &height, &channels);
		// Retrieve the raw image data from the texture
		void* data;
		texture->GetData(&data);
		// Generate texture and mipmaps on the GPU
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		textureApplied = true;
	}
	else
		perror("Texture is not valid!");
}

void Mesh::GetPivot(DVector3* pivotPoint)
{
	// if the vertex buffer for the mesh has already been established
	if (verts)
	{
		*pivotPoint = verts->pos;
		for (uint32_t i = 1; i < vertSize; i++)
			*pivotPoint += verts[i].pos;

		*pivotPoint /= vertSize;
	}
	else
		pivotPoint = nullptr;
}

void Mesh::GetShaderFromPath(const char* vertexPath, const char* fragmentPath)
{
	shader = new Shader(vertexPath, fragmentPath);
}

bool Mesh::operator==(const Graphic& other)
{
	const Mesh* tempCMP = dynamic_cast<const Mesh*>(&other);
	if (!tempCMP) return false;

	return memcmp(this, tempCMP, sizeof(Mesh)) == 0;
}

void Mesh::UpdateModelMatrixPos(const DMat4x4& _model)
{
	model = _model;
}

//void Mesh::ChangeColors()
//{
//
//	if (verts)
//	{		
//		for (uint32_t i = 0; i < vertSize; i++)
//		{
//			verts[i].color.r = (float)(rand() % 256 / 255);
//			verts[i].color.g = (float)(rand() % 256 / 255);
//			verts[i].color.b = (float)(rand() % 256 / 255);
//		}
//
//		// update the vertex buffer
//		glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize * sizeof(vertex), verts);
//	}
//}
