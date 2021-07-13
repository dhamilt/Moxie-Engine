#pragma once
#include "glPCH.h"
#include "SoundAsset.h"
#include "AudioClass.h"
class AudioSource : public AudioClass
{
	// Public functions
public: 
	AudioSource();
	AudioSource(MSoundDataAsset* _sound);
	AudioSource(ALCdevice* _device, ALCcontext* _context, MSoundDataAsset* _sound);
	~AudioSource();
	/// <summary>
	/// Adds sound to source
	/// </summary>
	/// <param name="_sound">New sound being added</param>
	void SetSound(MSoundDataAsset* _sound);
	/// <summary>
	/// Sets the volume of the audio source
	/// </summary>
	/// <param name="_volume"> New volume </param>
	void SetVolume(float _volume);
	/// <summary>
	/// Checks to see if the audio source is valid
	/// </summary>
	/// <returns> True if there is a sound loaded within it</returns>
	int IsValid();
	/// <summary>
	/// Plays the audio loaded into this source
	/// </summary>
	/// <param name="looping"></param>
	/// <returns></returns>
	int Play(bool looping = false);
	/// <summary>
	/// Returns the samples at the current time
	/// as a parameter
	/// </summary>
	/// <param name="sampleCount">The number of samples to retrieve</param>
	/// <param name="samples">The sample data</param>
	/// <returns>True if the audio source is playing, False if not</returns>
	int GetSamples(int sampleCount, float* samples);
	int GetSamples(int sampleCount, std::vector<float>& samples);
	int isPlaying();
	int Pause();
	int Stop();

	// public members
public:
	
	// private functions
private:
	ALenum GetAudioFormat();
	//private members
private:
	bool isLooping = false;
	float volume = 1.0f;
	MSoundDataAsset* sound = nullptr;
	ALint sourceState = NULL;
	ALuint bufferID = NULL;
	char* byteSampleArr = nullptr;
	short* sampleArr = nullptr;
	int currentSampleBufferSize = 0;
};

