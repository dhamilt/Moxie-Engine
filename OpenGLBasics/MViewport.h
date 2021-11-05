#pragma once
#include "GUI_Base.h"

// GUI element that mimics the pixel data of whats being drawn on GL viewport(s)
class MViewport : public GUI_Base
{
public:
	MViewport();
	~MViewport();
	virtual void Paint() override;
	bool operator==( GUI_Base& other);
	bool FrameBufferErrorCheck(GLuint status);
	

private:
	int width = 800, height = 600;
	GLuint fbo, rbo, viewportTexture;
	std::vector<GLuint> renderingLayers;
	uint8_t numOfRenderingLayers=0;
   std::vector<GLfloat> pixelData;
	GLuint pixelCount=0, colorChannelsPerPixel=4;
	size_t pixelBufferSize= 0;
	bool windowOpen;
};

