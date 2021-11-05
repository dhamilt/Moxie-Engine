#pragma once
#include "glPCH.h"

#define CHUNK_ID		4
#define CHUNK_SIZE		4
#define FORMAT_ID		4
#define SUBCHUNK_ID		4
#define SUBCHUNK_SIZE	4
#define AUDIO_FORMAT	2
#define NUM_OF_CHANNELS	2
#define SAMPLERATE		4
#define BYTERATE		4
#define BLOCKALIGN		2
#define BITS_PER_SAMPLE	2
#define SUBCHUNK2_ID	4
#define SUBCHUNK2_SIZE	4

class WavReader
{
public:
	WavReader() = delete;
	WavReader(std::string fileName);
	/// <summary>
	/// Loads the wave file data to the class
	/// </summary>
	/// <param name="fileName">File to load data from</param>
	void Load(std::string fileName);
	/// <summary>
	/// Checks to see if the wave file loaded is valid
	/// </summary>
	/// <returns>True if file is valid</returns>
	bool IsValid();
	/// <summary>
	/// Determines the audio format OpenAL should use to play this wave file
	/// </summary>
	ALuint RetrieveAudioFormat();

public:
	int chunkSize =			-1;
	int subChunkSize =		-1;
	int sampleRate =		-1;
	int byteRate =			-1;
	int subChunk2ndSize =	-1;
	short numChannels =		-1;
	short audioFormat =		-1;
	short bitsPerSample =	-1;
	short extraParamSize =	-1;
	short blockAlign =		-1;
	char* data = nullptr;

private:
	/// <summary>
	/// Reads file and retrieves buffer from it
	/// </summary>
	/// <param name="buffer">The char buffer being parsed from the file</param>
	/// <param name="bufferSize">The size of the buffer</param>
	/// <param name="file">The file being parsed</param>
	void ReadFile(char** buffer, size_t bufferSize, FILE* file);
	/// <summary>
	/// Reads file and retrieves buffer from it
	/// </summary>
	/// <param name="buffer">The int buffer being parsed from the file</param>
	/// <param name="bufferSize">The size of the buffer</param>
	/// <param name="file">The file being parsed</param>
	void ReadFile(int* buffer, size_t bufferSize, FILE* file);
	/// <summary>
	/// Reads file and retrieves buffer from it
	/// </summary>
	/// <param name="buffer">The short buffer being parsed from the file</param>
	/// <param name="bufferSize">The size of the buffer</param>
	/// <param name="file">The file being parsed</param>
	void ReadFile(short* buffer, size_t bufferSize, FILE* file);

private:
	bool isValid = true;
};

