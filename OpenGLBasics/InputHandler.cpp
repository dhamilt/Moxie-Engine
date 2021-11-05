#include "glPCH.h"
#include "InputHandler.h"
#include "GLSetup.h"
#include "Camera.h"
#include "SDL.h"


extern GLSetup* GGLSPtr;
InputHandler::InputHandler()
{
	leftMouseButton = new ButtonCommand();
	rightMouseButton = new ButtonCommand();
	moveCameraButton = new ButtonCommand();
	mouseCapture = new MouseCapture();
	scrollWheel = new AxisCommand();

	capturingCursor = true;
	leftMouseButton->Subscribe(std::bind(&MouseCapture::TrackMouseCursor, mouseCapture), MOX_HELD);
	//moveCameraButton->Subscribe(std::bind(&Camera::MoveCamera, GGLSPtr->mainCamera,)
}

InputHandler::InputHandler(bool captureMouseCursor)
{
	leftMouseButton = new ButtonCommand();
	rightMouseButton = new ButtonCommand();
	moveCameraButton = new ButtonCommand();
	scrollWheel = new AxisCommand();

	// if tracking the mouse position
	if (captureMouseCursor)
		mouseCapture = new MouseCapture();

	capturingCursor = captureMouseCursor;
}

InputHandler::~InputHandler()
{
	if (leftMouseButton)
		delete leftMouseButton;
	if (rightMouseButton)
		delete rightMouseButton;
	if (moveCameraButton)
		delete moveCameraButton;
	if (mouseCapture)
		delete mouseCapture;
	if (scrollWheel)
		delete scrollWheel;
}

bool InputHandler::PollInputEvents(SDL_Event* event)
{
	// Get all events from the events queue	
	bool result = SDL_PollEvent(event);
	
	// if any events triggered
	if (result)
	{
		SDL_MouseButtonEvent mouseEvent = event->button;
		
		// if either button is being held, call the appropriate delegates
		if (leftMouseButton->IsBeingHeld())
			leftMouseButton->OnHeld();

		if (rightMouseButton->IsBeingHeld())
			rightMouseButton->OnHeld();

		// if the event was a mouse wheel motion
		if (event->type == SDL_MOUSEWHEEL)
		{
			// retrieve how much the wheel has moved in which direction			
			SDL_MouseWheelEvent mwEvent = event->wheel;

			// Broadcast it to the mouse wheel scroll axis command
			scrollWheel->Execute(mwEvent.y);
		}

		// if the event was a keyboard button press
		if (event->type == SDL_KEYDOWN)
		{
			// if the button pressed was a camera move button
			const uint8_t* state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_A] || state[SDL_SCANCODE_S] || state[SDL_SCANCODE_D])
				// add the button press results
				HandleCameraMovement(state);			
			
		}

		// if the event was a keyboard button release
		if (event->type == SDL_KEYUP)
		{

		}

		// if the event was a mouse button press
		if (event->type == SDL_MOUSEBUTTONDOWN)
		{
			// check to see if the left mouse button triggered the event			
			if (mouseEvent.button == SDL_BUTTON_LEFT)			
				// if pressed
				if (mouseEvent.state == SDL_PRESSED)					
					// execute the left mouse pressed delegate
					leftMouseButton->OnPressed();		
				
			

			// check to see if the right mouse button triggered the event
			if (mouseEvent.button == SDL_BUTTON_RIGHT)
				// if pressed
				if (mouseEvent.state == SDL_PRESSED)
					// execute the left mouse pressed delegate
					rightMouseButton->OnPressed();
				
		}
		
		// if the event was a mouse button release
		if (event->type == SDL_MOUSEBUTTONUP)
		{
			// if the left mouse button triggered the event
			if (mouseEvent.button == SDL_BUTTON_LEFT)
				// if released
				if (mouseEvent.state == SDL_RELEASED)
					// execute the left mouse released delegate
					leftMouseButton->OnReleased();

			// if the right mouse button triggered the event
			if (mouseEvent.button == SDL_BUTTON_RIGHT)
				// if released
				if (mouseEvent.state == SDL_RELEASED)
					// execute the left mouse released delegate
					rightMouseButton->OnReleased();

			
		}
	}

	return result;
}

bool InputHandler::IsCapturingCursor()
{
	return capturingCursor;
}

void InputHandler::HandleCameraMovement(const uint8_t* _state)
{
	// Initial value for the direction
	unsigned int result = 0;

	// Combine the directions to the final movement of the camera
	if (_state[SDL_SCANCODE_W])
		result |= MOX_FORWARD;

	if (_state[SDL_SCANCODE_A])
		result |= MOX_LEFT;

	if (_state[SDL_SCANCODE_S])
		result |= MOX_BACKWARD;

	if (_state[SDL_SCANCODE_D])
		result |= MOX_RIGHT;

	// Tell the main camera to move
	GGLSPtr->mainCamera->MoveCamera(result);
}
