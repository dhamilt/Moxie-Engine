#include "glPCH.h"
#include "Mesh.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;
void Mesh::CreateVertexBuffer(const vector3* positions, const vector3* normals, const vector2* uvs)
{
	
	for (uint32_t i = 0; i < vertSize; i++)
	{
		vertex temp;

		temp.pos = positions[i];

		// if the mesh has uvs
		if (uvs)
			temp.texCoord = uvs[i];

		temp.normal = normals[i];

		verts.push_back(temp);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertSize, &verts[0], GL_STATIC_DRAW);
	// the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indexSize, &indexBuf[0], GL_STATIC_DRAW);

	// Add vertex positions for model
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, // start index
		3, // number of dimensions
		GL_FLOAT, // vertex buffer type
		GL_FALSE, // normalized?
		sizeof(vertex), // stride
		(void*)offsetof(vertex, vertex::pos) // buffer offset
	);

	// Add vertex texture coordinates for model	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		(void*)offsetof(vertex, vertex::texCoord)
	);

	// Add vertex normals for model	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, // layout index
		3, // number of dimensions
		GL_FLOAT, // value type for dimensions
		GL_FALSE, // normalized?
		sizeof(vertex), // stride
		(void*)offsetof(vertex, vertex::normal)// buffer offset
	);

	// Add vertex colors for model
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, // layout index
		3, // number of dimensions
		GL_FLOAT, // value type for dimensions
		GL_FALSE, // normalized
		sizeof(vertex), // stride
		(void*)offsetof(vertex, vertex::color) // buffer offset
	);
}

void Mesh::Draw(mat4 projection, mat4 view)
{
	// retrieve the current render pass 
	uint8_t currentRenderPass = 0;
	GetCurrentRenderPass(currentRenderPass);
	// retrieve the up to date light info
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
		shader->SetBool("isAlbedoTexValid", textureApplied);
		shader->SetFloat("exposure", 5.0f);
		// tells shader to render two pass Gaussian blur if on second two render passes
		shader->SetBool("isBlurring", currentRenderPass > 0 && currentRenderPass < 3);
		// informs shader to render the second pass of the Gaussian blur
		shader->SetBool("horizontal", currentRenderPass == 2);
		// Temporarily set the boolean values for specular and diffuse texture validation to false
		// TODO: Create a material struct that will contain all of these rendering properties
		shader->SetBool("isDiffuseTexValid", false);
		shader->SetBool("isSpecularTexValid", false);
		
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
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if(textureApplied)
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

void Mesh::GetPivot(vector3* pivotPoint)
{
	// if the vertex buffer for the mesh has already been established
	if (verts.size() > 0)
	{
		*pivotPoint = verts[0].pos;
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

void Mesh::SetMateialColor(const MLinearColor& color)
{
	materialColor = color;
}

void Mesh::UpdateModelMatrixPos(const mat4& _model)
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
