#pragma once
#include "glPCH.h"

struct Sound
{
	Sound()
	{
		if (_id == NULL)
			_id = 0;
		else
			_id++;
			
		id = _id; 
	}
	Sound(bool il, size_t al, void* _data) : isLooping(il), audioLength(al) 
	{
		if (_id == NULL)
			_id = 0;
		else
			_id++;

		id = _id;

		data = data;
	}
	bool isLooping = false;
	size_t audioLength = 0;
	void* data = 0;
	bool operator==(const Sound& other)
	{
		return(this->id == other.id && memcmp(this->data, other.data, audioLength) == 0);
	}
	
private:
	int id;
	static int _id;
};