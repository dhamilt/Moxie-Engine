#include "glPCH.h"
#include "MouseCapture.h"
#include "GLSetup.h"

extern GLSetup* GGLSPtr;
MouseCapture::MouseCapture()
{
	mcDelegate = MouseCaptureSignature();
}

MouseCapture::~MouseCapture()
{
	mcDelegate.UnsubscribeAll();
}

void MouseCapture::TrackMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	double _x = 0.0f, _y = 0.0f;
	
	// Get the mouse movement delta from ImGui
	_x = io.MouseDelta.x;
	_y = io.MouseDelta.y;

	// Get the current window dimensions
	GGLSPtr->GetWindowDimensions(width, height);

	// Get mouse delta relative to screen resolutions
	double relX = _x / (double)width;
	double relY = _y / (double)height;

	// Offset desired position by relative mouse position
	desiredPosX += relX;
	desiredPosY += relY;

	// Broadcast desired position to all listeners
	BroadcastMouseInput(&desiredPosX, &desiredPosY);	
}

void MouseCapture::Subscribe(MouseCapCallback param)
{
	mcDelegate += param;
}

void MouseCapture::Unsubscribe(MouseCapCallback param)
{
	mcDelegate -= param;
}

void MouseCapture::BroadcastMouseInput(double*x, double* y)
{
	mcDelegate.Broadcast(*x, *y);
}
