#include "glPCH.h"
#include "WavReader.h"
#include <cstdio>


WavReader::WavReader(std::string fileName)
{
	Load(fileName);	
}

void WavReader::Load(std::string fileName)
{
	FILE* file;
	// open the parameter file
	file = fopen(fileName.c_str(), "rb");
	// if the file is valid
	if (file)
	{
		char* charBuf = nullptr;
		// Read for the ChunkID
		ReadFile(&charBuf, CHUNK_ID, file);

		// Check to see if descriptor is correct
		if ( strcmp(charBuf, "RIFF")!= 0)
		{
			printf("File is not valid! ChunkID incorrect!\n");
			isValid = false;
			return;
		}

		// Read for chunk size
		ReadFile(&chunkSize, 1, file);

		// Read for FormatID
		free(charBuf);
		ReadFile(&charBuf, FORMAT_ID, file);

		// Check to see if descriptor is correct
		if (strcmp(charBuf, "WAVE") != 0)
		{
			printf("File is not valid! FormatID is incorrect!\n");
			isValid = false;
			return;
		}

		// Read for SubChunkID
		free(charBuf);
		ReadFile(&charBuf, SUBCHUNK_ID, file);

		// Check to see if descriptor is correct
		if (strcmp(charBuf, "fmt ") !=0)
		{
			printf("File is not valid! SubchunkID is incorrect!\n");
			isValid = false;
			return;
		}

		// Read for SubChunkSize
		ReadFile(&subChunkSize, 1, file);

		// Read for AudioFormat
		ReadFile(&audioFormat, 1, file);

		// Read for Number of Channels
		ReadFile(&numChannels, 1, file);

		// Read for Sample Rate
		ReadFile(&sampleRate, 1, file);

		// Read for Byte Rate
		ReadFile(&byteRate, 1, file);

		// Read for Block Align
		ReadFile(&blockAlign, 1, file);

		// Read for Bits Per Sample
		ReadFile(&bitsPerSample, 1, file);

		// Read for SubChunk2ID
		free(charBuf);
		ReadFile(&charBuf, SUBCHUNK2_ID, file);

		// Check to see if descriptor is correct
		if (strcmp(charBuf, "data") != 0)
		{
			printf("File is not valid! Subchunk2ID is incorrect!\n");
			isValid = false;
			return;
		}

		// Read for SubChunk2Size
		ReadFile(&subChunk2ndSize, 1, file);

		ReadFile(&data, subChunk2ndSize, file);

		// close the file
		fclose(file);
	}
	else
	{
		printf("File not found!");
		isValid = false;
	}
}

bool WavReader::IsValid()
{
	return isValid;
}

ALuint WavReader::RetrieveAudioFormat()
{
	if (bitsPerSample == 8 && numChannels == 1)
		return AL_FORMAT_MONO8;
	if (bitsPerSample == 8 && numChannels >= 2)
		return AL_FORMAT_STEREO8;
	if (bitsPerSample == 16 && numChannels == 1)
		return AL_FORMAT_MONO16;
	else
		return AL_FORMAT_STEREO16;
}

void WavReader::ReadFile(char** buffer, size_t bufferSize, FILE* file)
{
	*buffer = (char*)calloc(bufferSize+1, sizeof(char));
	if (*buffer)
	{
		(*buffer)[bufferSize] = '\0';
		fread(*buffer, sizeof(char), bufferSize, file);
	}
}

void WavReader::ReadFile(int* buffer, size_t bufferSize, FILE* file)
{
	fread(buffer, sizeof(int), bufferSize, file);
}

void WavReader::ReadFile(short* buffer, size_t bufferSize, FILE* file)
{
	fread(buffer, sizeof(short), bufferSize, file);
}
