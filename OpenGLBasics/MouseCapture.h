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
	void Subscribe(MouseCapCallback param);
	void Unsubscribe(MouseCapCallback param);
	void BroadcastMouseInput(double* x, double* y);

private:
	MouseCaptureSignature mcDelegate;
	int width, height;
};

