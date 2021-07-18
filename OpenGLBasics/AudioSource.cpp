#include "glPCH.h"
#include "AudioSource.h"
#include "BOM.h"

AudioSource::AudioSource()
{

}

AudioSource::AudioSource(MSoundDataAsset* _sound)
{
	SetSound(_sound);
}

AudioSource::AudioSource(ALCdevice* _device, ALCcontext* _context, MSoundDataAsset* _sound)
{	
	SetSound(_sound);
}



AudioSource::~AudioSource()
{
	// Free up any cached sample data
	if (sampleArr)
		free(sampleArr);
	if (byteSampleArr)
		free(byteSampleArr);

	//// Delete the buffer
	//alDeleteBuffers(1, &bufferID);
	//
	//// Delete the source
	//alDeleteSources(1, &sourceID);

	// Remove the resource 
	FreeSource();
}

void AudioSource::SetSound(MSoundDataAsset* _sound)
{
	// The set sound
	sound = _sound;

	//// Set the context to this one
	//if (!context)
	//	context = alcCreateContext(device, NULL);

	//// Set the current context to this one
	//if (context != alcGetCurrentContext())
	//	alcMakeContextCurrent(context);

	//// Clear the error queue
	//alGetError();
	//if(bufferID == NULL)
	////// Generate a buffer id
	//	alGenBuffers(1, &bufferID);

	//if (sourceID == NULL)
	//	// Generate source id
	//	alGenSources(1, &sourceID);

	//// Add buffer data
	//alBufferData(bufferID, sound->GetAudioFormat(), sound->data, sound->songChunkSize, sound->frequency);
	//
	//// Add buffer to the source
	//alSourcei(sourceID, AL_BUFFER, bufferID);

	// Add audio to the source
	LoadAudio(sound);
	
}

void AudioSource::SetVolume(float _volume)
{
	volume = glm::clamp<float>(_volume, 0.0f, 1.0f);
	SetGain(volume);
}

int AudioSource::IsValid()
{	
	return sound? 1 : 0;
}

int AudioSource::Play(bool looping)
{
	AudioClass::Play(looping);
	GetAudioState(&sourceState);
	

	if (sourceState == AL_PLAYING)
		return 1;
	else
		return 0;
}

int AudioSource::GetSamples(int sampleCount, float* samples)
{
	if (sourceState == AL_PLAYING)
	{
		ALint byteOffset;
		GetByteOffset(&byteOffset);
		int maximumByteOffsetAllowed = sound->songChunkSize - sampleCount;
		// As long as the audio class was able to retrieve the byte offset of where 
		// the audio file is currently, and the byte offset is less than or equal to
		// the maximum byte offset possible given the current sample count
		if (byteOffset != NULL && byteOffset<= maximumByteOffsetAllowed)
		{
			// Get the sample size being requested in bytes based on the number of channels provided
			auto sampleSize = (sound->bitsPerSample / 8) * sampleCount * sound->numOfChannels;
			/*char* sampleBufAtOffset = (char*)malloc(sampleSize);*/
			// If the new sample size doesn't match the cached sample size
			if (sampleSize != currentSampleBufferSize)
			{
				// if the sample byte array has already initialized
				if (byteSampleArr)
				{
					// Reallocate it to the new size
					byteSampleArr = (char*)realloc(byteSampleArr, sampleSize);
					assert(byteSampleArr);
				}
				
				// otherwise allocate it to the new size
				else
					byteSampleArr = (char*)malloc(sampleSize);

				// if the sample array has already been initialized
				if (sampleArr)
				{
					// Reallocate it to the new size
					sampleArr = (short*)realloc(sampleArr, sampleSize);
					assert(sampleArr);
				}
				// otherwise allocate it to the new size
				else
					sampleArr = (short*)malloc(sampleSize);

				// and cache the new size of the buffer
				currentSampleBufferSize = sampleSize;
			}
			
			// if the sample byte buffer has been initialized
			if (byteSampleArr)
			{
				// copy the offset audio data from the audio buffer
				// to the sample byte array
				memcpy(byteSampleArr, ((char*)sound->data) + byteOffset, sampleSize);
				
				if (sampleArr)
				{
					bool bigEndian = BOM::IsBigEndian();
					int numOfChannels = sound->numOfChannels;
					int sampleIndex = 0;
					for (int i = 0; i < sampleSize; i++)
					{
						memcpy(sampleArr + i, byteSampleArr, sampleSize);
						if (i % numOfChannels == numOfChannels - 1)
						{
							short avg = 0;
							for (int j = 0; j < numOfChannels; j++)
								avg += *(sampleArr + i - numOfChannels + j);



							float sampleFrameBothChannels = (float)avg / 32768;
							*(samples + sampleIndex) = sampleFrameBothChannels / numOfChannels;
							sampleIndex++;

						}
					}
										
					return 1;
				}
				
			}
		}
	}	
	return 0;
}

int AudioSource::GetSamples(int sampleCount, std::vector<float>& samples)
{
	if (sourceState == AL_PLAYING)
	{
		ALint byteOffset;
		GetByteOffset(&byteOffset);
		int maximumByteOffsetAllowed = sound->songChunkSize - sampleCount;
		// As long as the audio class was able to retrieve the byte offset of where 
		// the audio file is currently, and the byte offset is less than or equal to
		// the maximum byte offset possible given the current sample count
		if (byteOffset != NULL&& byteOffset <= maximumByteOffsetAllowed)
		{
			// Get the sample size being requested in bytes based on the number of channels provided
			auto sampleSize = (sound->bitsPerSample / 8) * sampleCount * sound->numOfChannels;
			/*char* sampleBufAtOffset = (char*)malloc(sampleSize);*/
			// If the new sample size doesn't match the cached sample size
			if (sampleSize != currentSampleBufferSize)
			{
				// if the sample byte array has already initialized
				if (byteSampleArr)
				{
					// Reallocate it to the new size
					byteSampleArr = (char*)realloc(byteSampleArr, sampleSize);
					assert(byteSampleArr);
				}

				// otherwise allocate it to the new size
				else
					byteSampleArr = (char*)malloc(sampleSize);

				// if the sample array has already been initialized
				if (sampleArr)
				{
					// Reallocate it to the new size
					sampleArr = (short*)realloc(sampleArr, sampleSize);
					assert(sampleArr);
				}
				// otherwise allocate it to the new size
				else
					sampleArr = (short*)malloc(sampleSize);

				// and cache the new size of the buffer
				currentSampleBufferSize = sampleSize;
			}

			// if the sample byte buffer has been initialized
			if (byteSampleArr)
			{
				// copy the offset audio data from the audio buffer
				// to the sample byte array
				memcpy(byteSampleArr, ((char*)sound->data) + byteOffset, sampleSize);

				if (sampleArr)
				{
					bool bigEndian = BOM::IsBigEndian();

					if (samples.size() > 0)
						samples.clear();
					int numOfChannels = sound->numOfChannels;
					int sampleArraySize = 0;
					for (int i = 0; i < sampleSize; i++)
					{
						// for every two bytes
						if (i % 2 == 0)
						{
							short result = 0;
							// Check to see if machine is in big endian
							if (bigEndian)
							{
								// reverse the byte order for the data
								char reverseBOM[] = { *(byteSampleArr + i), *(byteSampleArr + i - 1) };
								// then convert to a short								
								memcpy(&result, reverseBOM, sizeof(short));								
							}
							else
								// otherwise just convert to a short
								memcpy(&result, byteSampleArr + i, sizeof(short));

							// then add to sample collection
							*(sampleArr + sampleArraySize) = result;
							sampleArraySize++;
						}
						
					}

					
					for (int j = 0; j < sampleArraySize; j++)
					{
						if (j % numOfChannels == numOfChannels - 1)
						{
							short avg = 0;
							for (int k = 0; k < numOfChannels; k++)
							{
								avg += *(sampleArr + j - k);
							}
							float sampleFrameFromAllChannels = (float)avg / 32768;
							samples.push_back(sampleFrameFromAllChannels / numOfChannels);
						}
					}
					
					return 1;
				}

			}
		}
	}
	return 0;
}

int AudioSource::isPlaying()
{
	return sourceState == AL_PLAYING;
}

int AudioSource::Pause()
{
	// As long as there is a sound playing
	if (isPlaying())
	{		
		// Pause audio on source
		ALint result =AudioClass::Pause();
		GetAudioState(&sourceState);
		return result;
	}
	return 0;
}

int AudioSource::Stop()
{
	// if source is playing audio or paused
	if (isPlaying() || sourceState == AL_PAUSED)
	{
		// Stop audio from source
		AudioClass::Stop();
		GetAudioState(&sourceState);
		return sourceState == AL_STOPPED;
	}
	return 0;
}


