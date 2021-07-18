#include "glPCH.h"
#include "Cubemaps.h"
#include "ImageLibrary.h"
#include "GLSetup.h"


extern GLSetup* GGLSPtr;
Cubemaps::Cubemaps(std::vector<std::string> files)
{
	type = typeid(*this).name();
	// create space on the GPU for the cubemap as well as an identifier for it	
	glGenTextures(1, &cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	int width, height, channels;
	void* data;
	// for each image of the cubemap
	for (int i = 0; i < files.size(); i++)
	{
		std::string file = files[i];
		// read and extract data from the file name
		ImageLibrary::GetDataFromFile(file.c_str(), &data, &width, &height, &channels);
		textures.push_back(data);
		// then generate the portion of the cubemap the face is based on
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}

	// ensures that the textures on the cubemap are properly wrapped
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (cubemapID)
	{
		valid = true;
		AddToBuffer();
	}

}

Cubemaps::~Cubemaps()
{
	if (textures.size() > 0)
		for (std::vector<void*>::iterator it = textures.begin(); it != textures.end(); it++)
			ImageLibrary::FreeMemory(&(*it));
}

bool Cubemaps::IsValid()
{
	return valid;
}

void Cubemaps::AddToBuffer()
{
	// Generate the buffers for the vertex array and
	// the vertex buffer objects for the cubemap
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind the array object
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, &skyboxVertices[0], GL_STATIC_DRAW);

	// Pass vertex data to GPU for shaders
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float)*3,
		(void*)0
		
	);	

}

void Cubemaps::Draw(mat4 projection, mat4 view)
{
	glDepthMask(GL_FALSE);
	if (skyboxShader)
	{
		skyboxShader->Use();
		// Convert view matrix from mat4 to mat3 then back to mat4
		// again in order to remove the translation
		mat4 _view = mat4(mat3(view));
		skyboxShader->SetMat4("view", _view);
		skyboxShader->SetMat4("projection", projection);
	}

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}

void Cubemaps::GetShaderFromPath(const char* vsPath, const char* fsPath)
{
	skyboxShader = new Shader(vsPath, fsPath);
}

bool Cubemaps::operator==(const Graphic& other)
{
	const Cubemaps* cmCMP = dynamic_cast<const Cubemaps*>(&other);
	if (!cmCMP) return false;
	return memcmp(cmCMP, this, sizeof(Cubemaps)) == 0;
}
