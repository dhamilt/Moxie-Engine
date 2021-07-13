#include "glPCH.h"
#include "Texture.h"
#include "ImageLibrary.h"

Texture::Texture(const char* filePath)
{
	textureData = new TextureData();
	ImageLibrary::GetDataFromFile(filePath, &textureData);	
}

Texture::Texture(const char* filePath, TextureData* tData)
{
	ImageLibrary::GetDataFromFile(filePath, &tData);
	textureData = tData;
}

Texture::Texture(TextureData* tData)
{
	textureData = tData;
}

void Texture::GetAttributes(int* _width, int* _height, int* _channels)
{
	if (textureData)
	{
		*_width = textureData->width;
		*_height = textureData->height;
		*_channels = textureData->channels;
	}
	else
	{
		_width = nullptr;
		_height = nullptr;
		_channels = nullptr;
	}
}

void Texture::Bind()
{
	if (this->IsValid())
	{
		if (this->textureID == NULL)
		{
			glGenTextures(1, &this->textureID);
			glBindTexture(GL_TEXTURE_2D, this->textureID);

			int width, height, channels;
			this->GetAttributes(&width, &height, &channels);

			void* data;
			this->GetData(&data);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			glBindTexture(GL_TEXTURE_2D, this->textureID);
	}
}
