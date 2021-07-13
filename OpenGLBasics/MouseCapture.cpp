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
	int _x = 0, _y = 0;
	SDL_GetMouseState(&_x, &_y);
	if(width==NULL)
		GGLSPtr->GetWindowDimensions(width, height);
	double relX = (double)_x / (double)width;
	double relY = (double)_y / (double)height;

	printf("\rX: %f, Y: %f", relX, relY);
	BroadcastMouseInput(&relX, &relY);	
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
