#include "glPCH.h"
#include "AudioClass.h"

AudioClass::~AudioClass()
{
	if (sourceID != NULL)
		FreeSource();

	if (context)
		context = nullptr;
	
}

void AudioClass::LoadAudio(MSoundDataAsset* data)
{
	
	if (sourceID != NULL)
	{
		if (context != alcGetCurrentContext())
			alcMakeContextCurrent(context);
		alGenBuffers(1, &buffer);
		alBufferData(buffer, data->GetAudioFormat(), data->data, data->songChunkSize, data->frequency);
		
		alSourcei(sourceID, AL_BUFFER, buffer);
		GSoundEngine->CheckForErrors(alGetError());
	}

	

}

int AudioClass::FreeSource()
{
	alSourcei(sourceID, AL_BUFFER, 0);
	alDeleteBuffers(1, &buffer);
	return GSoundEngine->ReleaseAudioSource(&sourceID);
	 
}

void AudioClass::SetGain(float volume)
{
	alSourcef(sourceID, AL_GAIN, volume);
}

void AudioClass::GetAudioState(ALint* state)
{
	*state = audioState;
}

void AudioClass::GetByteOffset(ALint* offset)
{
	alGetSourcei(sourceID, AL_BYTE_OFFSET, offset);
}

int AudioClass::Play(bool loop)
{	
	return GSoundEngine->Play(sourceID, audioState, loop);
}

int AudioClass::Pause()
{
	alSourcePause(sourceID);
	alGetSourcei(sourceID, AL_SOURCE_STATE, &audioState);
	return audioState == AL_PAUSED;
}

int AudioClass::Stop()
{
	alSourceStop(sourceID);
	alSourcei(sourceID, AL_SOURCE_STATE, audioState);
	return audioState == AL_STOPPED;
}
