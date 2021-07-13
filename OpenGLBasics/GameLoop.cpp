#include "glPCH.h"
#include "GameLoop.h"
#include "GLSetup.h"
#include "SDL.h"

extern GLSetup* GGLSPtr;
extern SoundEngine* GSoundEngine;
extern GameLoop* GGLPtr = new GameLoop();

GameLoop::GameLoop()
{
	printf("There are a total of %d threads allotted on this machine.\n", MAX_NUM_OF_THREADS);

	isLooping = true;
	lastFrame = high_resolution_clock::now();	

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("SDL could not initialize! SDL_ERROR: %s\n", SDL_GetError());
	
	// Initialize the input handling class
	inputHandler = new InputHandler();
		
	SDL_CaptureMouse(SDL_TRUE);

	if(inputHandler->mouseCapture)
		mouseCursor = inputHandler->mouseCapture;
	
	
}

void GameLoop::QuitLoop()
{
	isLooping = false;
}

void GameLoop::InitializeAudio()
{
	// Start up the audio thread
	audioThread = std::thread(&SoundEngine::Loop, GSoundEngine);
	// then detach it
	audioThread.detach();
}


bool GameLoop::Loop()
{	
	SDL_Event event;
	while (isLooping)
	{
		// Poll the events		
		if (inputHandler->PollInputEvents(&event))
		{
			if (event.type == SDL_QUIT)
			{
				isLooping = false;
				break;
			}			

			//// Get all Keyboard Events
			const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

			// if the escape key is pressed
			if (keyboardState[SDL_SCANCODE_ESCAPE])
			{
				// exit the current loop and the main game loop
				isLooping = false;
				break;
			}
						
		}

		// Initialize the audio
		/*if (!audioInitialized)
		{
			InitializeAudio();
			audioInitialized = true;
		}*/
		// Initialize the Log window for the project
		// Once the main window is loaded
		/*if (!logWindow && GGLSPtr->windowLoaded)
			logWindow = &Log::Get();*/
		// Calculate the time since the last frame
		double _deltaTime;
		high_resolution_clock::time_point currentFrame = high_resolution_clock::now();
		duration<double> timeSpan = duration_cast<duration<double>>(currentFrame - lastFrame);
		_deltaTime = timeSpan.count();

		// Call all subscribers' Update functions with the current deltaTime		
			for (int i = 0; i < objsInLoop.size(); i++)
				if(objsInLoop[i])
					objsInLoop[i]->Update(_deltaTime);

		// Set the current frame time as the last frame
		lastFrame = currentFrame;

		// Tell the Rendering Thread to render one frame
		if(GGLSPtr)
			GGLSPtr->Render();
	}

	return true;
}

void GameLoop::operator+=(BaseObject* ptr)
{		
		objsInLoop.push_back(ptr);	
}

void GameLoop::AddMouseCapCallback(std::function<void(double, double)> callback)
{
	if (inputHandler)
	{
		MouseCapture* mouseCapture = inputHandler->mouseCapture;

		if (mouseCapture)
			inputHandler->mouseCapture->Subscribe(callback);
	}
}

void GameLoop::RemoveMouseCapCallback(std::function<void(double, double)> callback)
{
	if (inputHandler)
	{
		MouseCapture* mouseCapture = inputHandler->mouseCapture;

		if (mouseCapture)
			inputHandler->mouseCapture->Unsubscribe(callback);
	}
}

InputHandler* GameLoop::GetMainInputHandle()
{
	return inputHandler;
}


