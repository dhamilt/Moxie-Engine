#pragma once
#include <atomic>
#include <vector>
#include <al.h>
#include <alc.h>
#include "ObjectPool.h"

// TODO: Create an object pool for audio source ids
// that way when the resources for one audio source
// released, they can be used by another
class SoundEngine
{
public:
	// Initializes the sound engine
	SoundEngine();
	~SoundEngine();
	// Starts the loop that will run the sound engine
	void Init();

	// Ends the sound loop and causes the audio thread to end
	void Quit();

	// Generates an audio source for the audio class being initialized
	void GenerateAudioSource(ALuint& _sourceID);

	// Function that allows audio classes to request to play audio
	void RequestForAudio(ALuint& _sourceID, ALuint& _bufferID);

	// Release audio source id from the audio class using it
	bool ReleaseAudioSource(ALuint* _sourceID);
	
	// OpenAL Error Handling functionality
	void CheckForErrors(ALenum errorState);

	// Plays audio
	int Play(ALuint& source, ALint& state, bool looping = false);
	
	ALCcontext* GetContext();
	// Retrieves the list of devices available
	bool GetAllDevices();

private:
	// Loops through requests for audio
	void Loop();
	
	

private:
	// Thread safe variables
	/*std::atomic<*/ALCdevice* device;
	/*std::atomic<*/ALCcontext* context;
	std::vector<std::string> deviceList;
	// Non thread safe variables
	bool quit = false;
	ObjectPool<ALuint> sources;


	friend class GameLoop;
};

// Global pointer for the sound engine
extern SoundEngine* GSoundEngine;