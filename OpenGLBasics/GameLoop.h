#pragma once
#include <chrono>
#include "BaseObject.h"
#include <thread>
#include "SDL.h"
#include "InputHandler.h"
#include "SoundEngine.h"
#include "Log.h"

using namespace std::chrono;

//typedef void(*Update_ptr)(BaseObject*);

static const unsigned int MAX_NUM_OF_THREADS = std::thread::hardware_concurrency();

class GameLoop
{
public:
	
	GameLoop();
	void QuitLoop();
	// Starts up the audio engine
	void InitializeAudio();
	
	bool Loop();
	

	void operator+=(BaseObject* ptr);
	/*Public functions for subscribing and unsubscribing to mouse capture*/
	void AddMouseCapCallback(std::function<void(double, double)> callback);
	void RemoveMouseCapCallback(std::function<void(double, double)> callback);
	InputHandler* GetMainInputHandle();
	// Function call for delay initialization of features
	// on main GameLoop class
	void Init();

public:
	Log* logWindow;
private:
	double _deltaTime;
	std::vector<BaseObject*> objsInLoop;	 		
	long subscriberCount = 0;
	high_resolution_clock::time_point lastFrame;
	bool isLooping;
	SDL_Window* window =nullptr;
	int width = 0, height = 0;	
	InputHandler* inputHandler = nullptr;
	MouseCapture* mouseCursor = nullptr;
	std::thread audioThread;
	bool audioInitialized;
	
};

extern GameLoop* GGLPtr;