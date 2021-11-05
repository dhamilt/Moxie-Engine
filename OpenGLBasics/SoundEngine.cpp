#include "glPCH.h"
#include "SoundEngine.h"
#include <thread>


extern SoundEngine* GSoundEngine = new SoundEngine();

SoundEngine::SoundEngine()
{
	GetAllDevices();
	Init();
	GetAllDevices();
	sources = ObjectPool<ALuint>(5, 150);
}

SoundEngine::~SoundEngine()
{
	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	// Free up contexts on the device
	alcMakeContextCurrent(NULL);
	// Delete the context
	alcDestroyContext(context);
	// Destroy the device
	alcCloseDevice(device);
	
	
	
	// Free up all sources on the context	
	std::vector<ALuint> removedSources = sources.RemoveAllObjects();
	for (int i = 0; i < removedSources.size(); i++)
	{
		// remove the buffer(s) from each source
		alSourcei(removedSources[i], AL_BUFFER, NULL);
		// Delete each source
		alDeleteSources(1, &removedSources[i]);
	}
	removedSources.clear();
	
}

void SoundEngine::Init()
{
	device = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
}

void SoundEngine::Quit()
{
	quit = true;
}

void SoundEngine::GenerateAudioSource(ALuint& _sourceID)
{
	alcMakeContextCurrent(context);
	sources.GetItem(_sourceID);
	// if there is an inactive audio source in the pool
	if (_sourceID != 0)
		// exit the function
		return;
	// Otherwise
	else
	{
		// Attempt to generate an audio source
		alGenSources(1, &_sourceID);

		// Error check
		ALenum result = alGetError();
		CheckForErrors(result);

		// Throw an assert if the audio source was not generated
		// and/or if there was an error generating it
		assert(&_sourceID && result == AL_NO_ERROR);

		// Otherwise add the source to the queue
		sources.AddToPool(_sourceID);
	}
}

void SoundEngine::RequestForAudio(ALuint& _sourceID, ALuint& _bufferID)
{
	if (_sourceID)
	{
		// Check to see if the source exists in the pool
		if (sources.Find(_sourceID == -1))
			// if not, add it
			sources.AddToPool(_sourceID);

		// load the source with the buffer data
		alSourcei(_sourceID, AL_BUFFER, _bufferID);
	}
}

bool SoundEngine::ReleaseAudioSource(ALuint* _sourceID)
{
	return sources.DeactivateInPool(*_sourceID);	
}

void SoundEngine::Loop()
{
	// for each iteration of the loop
	while (!quit)
	{
		//printf("Hi! I'm your audio thread!\n");
		// Go through each request for audio resources
		// if enough resources are available
		// Generate an audio source
		// Load the audio into a buffer for the audio source
		// play the audio
	}
	this->~SoundEngine();
}

void SoundEngine::CheckForErrors(ALenum errorState)
{
	switch (errorState)
	{
		case AL_NO_ERROR:			
			break;
		case AL_INVALID_NAME:
			perror("Error! Incorrect name!");
			break;
		case AL_INVALID_ENUM:
			perror("Error! Incorrect enumeration!");
			break;
		case AL_INVALID_VALUE:
			perror("Error! Incorrect value!!");
			break;
		case AL_INVALID_OPERATION:
			perror("Error! Incorrect operation!");
			break;
		case AL_OUT_OF_MEMORY:
			perror("Error! Not enough memory!");
			break;
	}
}

int SoundEngine::Play(ALuint& source, ALint& state, bool looping)
{
	context = alcGetCurrentContext();

	if (looping)
		alSourcei(source, AL_LOOPING, looping);
	alSourcePlay(source);
	CheckForErrors(alGetError());
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	CheckForErrors(alGetError());

	return state == AL_PLAYING;
}

ALCcontext* SoundEngine::GetContext()
{
	return context;
}

bool SoundEngine::GetAllDevices()
{
	// If there are no devices available
	if(!alcGetString(NULL, ALC_DEVICE_SPECIFIER))
		return false;

	printf("Devices: %s\n", alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	//// Otherwise add each one to the device list	
	//int i = 0;
	//while (true)
	//{

	//}
	return true;

}
