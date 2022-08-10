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
	// if the previous cursor position has been cached
	if (prevCursorPosX != -FLT_MAX && prevCursorPosY != -FLT_MAX)	
	{
		double _x = 0.0f, _y = 0.0f;
		DVector2 mouseDelta = DVector2(io.MousePos.x -prevCursorPosX, io.MousePos.y - prevCursorPosY);
		// Get the mouse movement delta from ImGui
		_x = mouseDelta.x;
		_y = mouseDelta.y;

		printf("\rMouse Delta: (%f, %f)", mouseDelta.x, mouseDelta.y);

		// Get the current window dimensions
		GGLSPtr->GetViewportDimensions(width, height);

		// Get mouse delta relative to screen resolutions
		double relX = _x / (double)width;
		double relY = _y / (double)height;

		// Offset desired position by relative mouse position
		desiredPosX += relX;
		desiredPosY += relY;


		// Broadcast desired position to all listeners
		BroadcastMouseInput(&desiredPosX, &desiredPosY);
	}
	// cache the current mouse position
	prevCursorPosX = io.MousePos.x;
	prevCursorPosY = io.MousePos.y;
}

void MouseCapture::ResetMouseCapture()
{
	prevCursorPosX = -FLT_MAX;
	prevCursorPosY = -FLT_MAX;
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
