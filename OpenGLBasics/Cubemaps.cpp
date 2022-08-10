#include "glPCH.h"
#include "Cubemaps.h"
#include "ImageLibrary.h"
#include "GLSetup.h"
#include "Material.h"


extern GLSetup* GGLSPtr;
Cubemaps::Cubemaps(std::vector<std::string> files)
{
	type = typeid(*this).name();
	//// create space on the GPU for the cubemap as well as an identifier for it	
	//glGenTextures(1, &cubemapID);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	std::vector<TextureData*> textureDataCache;
	
	// for each image of the cubemap
	for (int i = 0; i < files.size(); i++)
	{
		std::string file = files[i];
		TextureData* textureData = new TextureData();
		// read and extract data from the file name
		ImageLibrary::GetDataFromFile(file.c_str(), &textureData->data, &textureData->width,
									  &textureData->height, &textureData->channels);
		
		textureDataCache.push_back(textureData);		
	}

	GGLSPtr->SubmitCubeMapData(textureDataCache);

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

void Cubemaps::Draw(DMat4x4 projection, DMat4x4 view)
{
	//glDepthMask(GL_FALSE);
	//if (skyboxShader)
	//{
	//	skyboxShader->Use();
	//	// Convert view matrix from DMat4x4 to mat3 then back to DMat4x4
	//	// again in order to remove the translation
	//	DMat4x4 _view = DMat4x4(mat3(view));
	//	skyboxShader->SetMat4("view", _view);
	//	skyboxShader->SetMat4("projection", projection);
	//}

	//glBindVertexArray(vao);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);
	//glDepthMask(GL_TRUE);
}

void Cubemaps::SetMaterial(Material* mat)
{
	skyboxMat = mat;
	GGLSPtr->AddMaterialToPipeline("Skybox", mat);
}

void Cubemaps::GetShaderFromPath(const char* vsPath, const char* fsPath)
{
	if (!skyboxMat)
		skyboxMat = new Material();
	skyboxMat->SetShader(new Shader(vsPath, fsPath));

}

bool Cubemaps::operator==(const Graphic& other)
{
	const Cubemaps* cmCMP = dynamic_cast<const Cubemaps*>(&other);
	if (!cmCMP) return false;
	return memcmp(cmCMP, this, sizeof(Cubemaps)) == 0;
}
