#pragma once
#include <GL\glew.h>

namespace Moxie
{
	void GLErrorReporting();
	void GLErrorReporting(GLenum errorType);
	bool FrameBufferErrorCheck();
	bool FrameBufferErrorCheck(GLuint status);
}