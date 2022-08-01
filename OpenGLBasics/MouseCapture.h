#pragma once
#include "EventHandler.h"

typedef EventHandler<void, double, double>MouseCaptureSignature;
// Callback for mouse capture
typedef std::function<void(double, double)> MouseCapCallback;

class MouseCapture
{
public:
	MouseCapture();
	~MouseCapture();

	void TrackMouseCursor();
	// Resets the mouse capture cache
	void ResetMouseCapture();
	void Subscribe(MouseCapCallback param);
	void Unsubscribe(MouseCapCallback param);
	void BroadcastMouseInput(double* x, double* y);

private:
	MouseCaptureSignature mcDelegate;
	int width, height;
	float prevCursorPosX = -FLT_MAX, prevCursorPosY = -FLT_MAX;
	double desiredPosX = 0.0f, desiredPosY = 0.0f;
};

