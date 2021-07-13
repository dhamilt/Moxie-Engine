#pragma once
#include "ButtonCommand.h"
#include "AxisCommand.h"
#include "MouseCapture.h"
#include "SDL.h"

class InputHandler
{

public:
	// Member Functions
	// Captures mouse position by default
	InputHandler();
	InputHandler(bool captureMouseCursor);
	~InputHandler();
	bool PollInputEvents(SDL_Event* event);
	bool IsCapturingCursor();	

	// Member variables
	ButtonCommand* leftMouseButton = nullptr;
	ButtonCommand* rightMouseButton = nullptr;
	ButtonCommand* moveCameraButton = nullptr;
	AxisCommand* scrollWheel = nullptr;
	MouseCapture* mouseCapture = nullptr;

	// Member Functions
private:
	// Handles camera movement inputs
	void HandleCameraMovement(const uint8_t* _state);

private:
	bool capturingCursor;
};

