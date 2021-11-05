#pragma once
#include "TextureData.h"
#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ASSERT(x)
#include "stb_image.h"

class ImageLibrary
{
public:
	static void  GetDataFromFile(const char* filePath, void** data, int* width, int* height, int* channels);
	static void GetDataFromFile(const char* filePath, TextureData** data);
	static void FreeMemory(void** data);
};

inline void ImageLibrary::GetDataFromFile(const char* filePath, void** data, int* width, int* height, int* channels)
{
	stbi_set_flip_vertically_on_load(false);
	*data = stbi_load(filePath, width, height, channels, 0);
}

inline void ImageLibrary::GetDataFromFile(const char* filePath, TextureData** data)
{
	stbi_set_flip_vertically_on_load(false);
	(*data)->data = stbi_load(filePath, &(*data)->width, &(*data)->height, &(*data)->channels, 0);
}

inline void ImageLibrary::FreeMemory(void** data)
{
	stbi_image_free(*data);
}
