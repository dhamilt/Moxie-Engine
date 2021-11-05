#pragma once
#include "WavReader.h"

struct MSoundDataAsset
{
	MSoundDataAsset() = delete;
	MSoundDataAsset(std::string fileName)
	{
		WavReader* reader = new WavReader(fileName);
		if (reader->IsValid())
		{
			songName = fileName;
			frequency = reader->sampleRate;
			bitsPerSample = reader->bitsPerSample;
			byteRate = reader->byteRate;
			songChunkSize = reader->subChunk2ndSize;
			numOfChannels = reader->numChannels;
			data = reader->data;
			songLength = (float)songChunkSize / (float)byteRate;
		}
	}
	/// <summary>
	/// Default and only constructor for the sound data asset structure
	/// </summary>
	/// <param name="sn"> Song Name </param>
	/// <param name="freq"> Sample Rate </param>
	/// <param name="bps"> Bits per sample </param>
	/// <param name="br"> Byte Rate </param>
	/// <param name="scs"> Song Chunk Size </param>
	/// <param name="nmc"> Number of Channels </param>
	/// <param name="sl"> Song Length </param>
	/// <param name="data"> Actual data </param>
	MSoundDataAsset(std::string sn, int freq, int bps, int br, int scs, int nmc, float sl, void* _data) 
		: songName(sn), frequency(freq), bitsPerSample(bps), byteRate(br), songChunkSize(scs), numOfChannels(nmc), songLength(sl)
	{
		data = _data;
		songLength = (float)songChunkSize / (float)byteRate;
	};
	ALenum GetAudioFormat()
	{
		// if there is a sound asset in this audio source		
		if (numOfChannels >= 2 && bitsPerSample == 16)
			return AL_FORMAT_STEREO16;
		if (numOfChannels >= 2 && bitsPerSample == 8)
			return AL_FORMAT_STEREO8;
		if (numOfChannels == 1 && bitsPerSample == 16)
			return AL_FORMAT_MONO16;
		else
			return AL_FORMAT_MONO8;

		return NULL;
	}

	std::string songName;
	int frequency = 0;
	int bitsPerSample = 0;
	int byteRate = 0;
	int songChunkSize = 0;
	int numOfChannels = 0;
	float songLength = 0;
	void* data = 0;

};