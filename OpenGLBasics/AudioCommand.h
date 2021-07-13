#pragma once
#include "Command.h"
#include "Sound.h"
class AudioCommand :
    public Command<Sound*>
{
public:
    AudioCommand() = delete;
    AudioCommand(Sound* _sound) { sound = _sound; }
    virtual void Execute(Sound* sound) override;

private:
    Sound* sound;
};

