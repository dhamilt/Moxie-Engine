#include "glPCH.h"
#include "AssetImportLibrary.h"

GLuint AssetImportLibrary::LoadBMP(const char* imgPath)
{
	FILE* file = fopen(imgPath, "rb");
	if (file)
	{
		// the header of a BMP file is always exactly 54 bytes in size
		uint8_t header[54];
		// Position of the file where the data begins
		uint32_t dataPos;
		uint32_t wth, height;
		// The size of the image
		// The number of pixels (wth x height) multiplied by the number of color channels (3)
		uint32_t imageSize;
		// The actual texture data
		uint8_t* data;

		if (fread(header, sizeof(uint8_t), 54, file) != 54)
		{
			perror("Incorrect BMP file format!\n");
			return GL_FALSE;
		}

		if (header[0] != 'B' || header[1] != 'M')
		{
			perror("Incorrect BMP file format!\n");
			return GL_FALSE;
		}

		// read the ints from the byte array
		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		wth = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		// Some BMP files can be misformatted,
		// so fill with potential missing info
		imageSize = imageSize == 0 ? wth * height * 3 : imageSize;
		dataPos = dataPos == 0 ? 54 : dataPos;

		// Create data buffer
		data = (uint8_t*)calloc(imageSize, sizeof(uint8_t));

		if (data != 0)
			// Fill the buffer
			fread(data, sizeof(uint8_t), imageSize, file);

		// Create an OpenGL texture
		GLuint texture;
		glGenTextures(1, &texture);

		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// Feed the texture an image to render
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wth, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		// When magnifying the texture (because there are no mipmaps at this size), use linear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// When shrinking, the texture, use a linear blend of the two mipmaps it's closest to; each filtered linearly as well
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



		glGenerateMipmap(GL_TEXTURE_2D);

		free(data);
		fclose(file);
		return GL_TRUE;
	}

	perror("Image could not be loaded from path!\n");
	return GL_FALSE;
}
