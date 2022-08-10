#pragma once
#include "GUI_Base.h"

// GUI element that mimics the pixel data of whats being drawn on GL viewport(s)
class WViewport : public GUI_Base
{
public:
	WViewport();
	~WViewport();
	virtual void Paint() override;
	bool operator==( GUI_Base& other);
	bool FrameBufferErrorCheck(GLuint status);
	

private:
	int width = 800, height = 600;
	GLuint fbo, textureID, rbo;
	GLubyte* pixelData;
	GLuint pixelCount=0, bytesPerPixel=4;
	bool windowOpen;
};

