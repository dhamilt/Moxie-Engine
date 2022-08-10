#include "glPCH.h"


void Moxie::GLErrorReporting()
{
	GLErrorReporting(glGetError());
}

void Moxie::GLErrorReporting(GLenum errorType)
{
	switch (errorType)
	{
	case GL_NO_ERROR:
		printf("No error found.\n");
		break;

	case GL_INVALID_ENUM:
		printf("Invalid enum!\n");
		break;

	case GL_INVALID_VALUE:
		printf("Invalid value!\n");
		break;

	case GL_INVALID_OPERATION:
		printf("Invalid operation!\n");
		break;

	case GL_INVALID_FRAMEBUFFER_OPERATION:
		printf("Invalid framebuffer operation!\n");
		break;

	case GL_OUT_OF_MEMORY:
		printf("Out of memory!\n");
		break;

	case GL_STACK_UNDERFLOW:
		printf("Stack Underflow!\n");
		break;

	case GL_STACK_OVERFLOW:
		printf("Stack Overflow!\n");
		break;

	default:
		printf("No error found.\n");
		break;
	}
}

bool Moxie::FrameBufferErrorCheck()
{
	return FrameBufferErrorCheck(glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

bool Moxie::FrameBufferErrorCheck(GLuint status)
{
	bool result = false;

	switch (status)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		printf("\rFramebuffer is not defined.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		printf("\rFramebuffer attachments are incomplete.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		printf("\rFramebuffer does not have at least one image attached to it.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		printf("\rDraw buffer cannot not be found in framebuffer.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		printf("\rRead buffer cannot not be found in framebuffer.");
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		printf("\rThe internal formats for this framebuffer are not supported");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		printf("\rFramebuffer samples are not all the same for the attached render buffers.");
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		printf("\rFramebuffer is layered and the attachment is not.");
		break;

	case GL_FRAMEBUFFER_COMPLETE:
		result = true;
		printf("\rNo framebuffer error found.");
		break;

	default:
		result = true;
		break;
	}
	return result;
}
