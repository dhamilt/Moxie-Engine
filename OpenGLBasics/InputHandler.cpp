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
	rightMouseButton->Subscribe(std::bind(&MouseCapture::TrackMouseCursor, mouseCapture), MOX_HELD);
	rightMouseButton->Subscribe(std::bind(&MouseCapture::TrackMouseCursor, mouseCapture), MOX_PRESSED);
	rightMouseButton->Subscribe(std::bind(&MouseCapture::ResetMouseCapture, mouseCapture), MOX_RELEASED);
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

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			leftMouseButton->OnPressed();
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
			rightMouseButton->OnPressed();

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

bool InputHandler::PollInputEvents()
{
	// if left mouse button is being held
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		leftMouseButton->OnHeld();
	// if right mouse button is being held
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
		rightMouseButton->OnHeld();

	// if left mouse button was clicked
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		leftMouseButton->OnPressed();
	// if left mouse button was released
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		leftMouseButton->OnReleased();

	// if right mouse button was clicked
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		rightMouseButton->OnPressed();
	// if right mouse button was released
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		rightMouseButton->OnReleased();

	// If mouse scroll wheel was moved
	//if(ImGui::Mouse)

	uint8_t state = 0;
	// If a W,A,S,or D key was pressed
	if (ImGui::IsKeyPressed(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_W))
		state |= MOX_FORWARD;
	if (ImGui::IsKeyPressed(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_D))
		state |= MOX_RIGHT;
	if (ImGui::IsKeyPressed(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_S))
		state |= MOX_BACKWARD;
	if (ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_A))
		state |= MOX_LEFT;

	// if any WASD key was pressed
	if(state > 0)
		// Tell the main camera to move
		GGLSPtr->mainCamera->MoveCamera(state);

	return false;
}

bool InputHandler::IsCapturingCursor()
{
	return capturingCursor;
}

void InputHandler::WatchMouseInputs(MouseButtonFlags mouseButton, ButtonStateFlags buttonStateFlags)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Mouse button pressed queue events for ImGui
	if (buttonStateFlags & ButtonState_Pressed)
	{
		if (mouseButton & MouseButton_Left)
			io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
		if (mouseButton & MouseButton_Right)
			io.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
		if (mouseButton & MouseButton_Middle)
			io.AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
	}

	// Mouse button released queue events for ImGui
	if (buttonStateFlags & ButtonState_Released)
	{
		if (mouseButton & MouseButton_Left)
			io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
		if (mouseButton & MouseButton_Right)
			io.AddMouseButtonEvent(ImGuiMouseButton_Right, false);
		if (mouseButton & MouseButton_Middle)
			io.AddMouseButtonEvent(ImGuiMouseButton_Middle, false);
		
	}
	
}

void InputHandler::WatchMousePosition()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.AddMousePosEvent(0, 0);
}

void InputHandler::WatchMouseWheelPosition()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	io.AddMouseWheelEvent(1, 1);
}

void InputHandler::WatchKeyStroke(ImGuiKey keyStroke, ButtonStateFlags buttonStateFlags)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Button pressed queue events for ImGui
	if (buttonStateFlags & ButtonState_Pressed)
		io.AddKeyEvent(keyStroke, true);

	// Button released queue events for ImGui
	if (buttonStateFlags & ButtonState_Released)
		io.AddKeyEvent(keyStroke, false);
}

void InputHandler::WatchKeyStrokes(std::vector<ImGuiKey> keyStrokes, ButtonStateFlags buttonStateFlags)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	for (auto it = keyStrokes.begin(); it != keyStrokes.end(); it++)
	{
		ImGuiKey keyStroke = *it;
		// Button pressed queue events for ImGui
		if (buttonStateFlags & ButtonState_Pressed)
			io.AddKeyEvent(keyStroke, true);

		// Button released queue events for ImGui
		if (buttonStateFlags & ButtonState_Released)
			io.AddKeyEvent(keyStroke, false);
	}
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

void InputHandler::HandleCameraMovement()
{
	uint32_t result = 0;
	// Combine the directions to the final movement of the camera
	if (ImGui::IsKeyDown(ImGuiKey_W))
		result |= MOX_FORWARD;

	if (ImGui::IsKeyDown(ImGuiKey_A))
		result |= MOX_LEFT;

	if (ImGui::IsKeyDown(ImGuiKey_S))
		result |= MOX_BACKWARD;

	if (ImGui::IsKeyDown(ImGuiKey_D))
		result |= MOX_RIGHT;

	// Tell the main camera to move
	GGLSPtr->mainCamera->MoveCamera(result);
}
