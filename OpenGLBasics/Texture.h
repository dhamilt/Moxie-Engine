#pragma once
#include "ImageLibrary.h"





// TODO Create a Texture Asset Manager class that reads the raw data from image files, creates new textures, and handle memory resources for that Texture asset

class Texture
{
public:
	Texture(const char* filePath);
	Texture(const char* filePath, TextureData* tData);
	Texture(TextureData* tData);
	
	~Texture() { delete textureData; }

	bool IsValid() { return textureData; }
	void GetData(void** _data) { *_data = textureData ? textureData->data : nullptr; }
	void GetAttributes(int* _width, int* _height, int* _channels);
	void Bind();
	friend class SpriteRenderer;
private:
	TextureData* textureData = nullptr;
	Texture() {};
	unsigned int textureID;
};



