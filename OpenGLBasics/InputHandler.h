#pragma once
#include "ButtonCommand.h"
#include "AxisCommand.h"
#include "MouseCapture.h"
#include "SDL.h"

// Left = 1, Middle = 2, Right = 4
typedef int MouseButtonFlags;
enum MouseButtonFlags_
{
	MouseButton_Left	= 1,
	MouseButton_Middle	= 1 << 1,
	MouseButton_Right	= 1 << 2
};

// Pressed = 1, Released = 2, Held = 4
// NOTE: Held state does not current work
typedef int ButtonStateFlags;
enum ButtonStateFlags_
{
	ButtonState_Pressed		= 1,
	ButtonState_Released	= 1 << 1,
	ButtonState_Held		= 1 << 2
};

class InputHandler
{

public:
	// Member Functions
	// Captures mouse position by default
	InputHandler();
	InputHandler(bool captureMouseCursor);
	~InputHandler();
	bool PollInputEvents(SDL_Event* event);
	// "Polls" Input events fed through Dear ImGui
	bool PollInputEvents();
	bool IsCapturingCursor();

	// Request ImGui to watch for specific mouse inputs
	void WatchMouseInputs(MouseButtonFlags mouseButton, ButtonStateFlags buttonStateFlags);
	// Request ImGui to watch for mouse cursor positioning
	void WatchMousePosition();
	// Request ImGui to watch for mouse wheel scrolling
	void WatchMouseWheelPosition();
	// Request ImGui to watch for a specific keystroke
	void WatchKeyStroke(ImGuiKey keyStroke, ButtonStateFlags buttonStateFlags);
	// Request ImGui to watch for a range of specific keystrokes
	void WatchKeyStrokes(std::vector<ImGuiKey> keyStrokes, ButtonStateFlags buttonStateFlags);

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
	void HandleCameraMovement();

private:
	bool capturingCursor;
};

