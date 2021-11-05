#pragma once
#include "SoundEngine.h"
#include "SoundAsset.h"
class AudioClass
{
public:
	AudioClass() { GSoundEngine->GenerateAudioSource(sourceID); context = GSoundEngine->GetContext(); }
	~AudioClass();
	void LoadAudio(MSoundDataAsset* data);
	int FreeSource();
	void SetGain(float volume);
	void GetAudioState(ALint* state);
	void GetByteOffset(ALint* offset);
	int Play(bool loop = false);
	int Pause();
	int Stop();


private:
	ALuint sourceID;
	ALuint buffer;
	ALint audioState;
	ALCcontext* context;
};

extern SoundEngine* GSoundEngine;