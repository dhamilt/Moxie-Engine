#pragma once
#include <GL/GL.h>
#include <cstdio>

namespace Moxie
{
		static void GLErrorCheck(unsigned int status)
		{
			switch (status)
			{
			case GL_NO_ERROR:
				printf("No error found.");
				break;
			case GL_INVALID_ENUM:
				printf("Invalid Enum!");
				break;
			case GL_INVALID_VALUE:
				printf("Invalid Value!");
				break;
			case GL_INVALID_OPERATION:
				printf("Invalid Operation!");
				break;
			case GL_STACK_OVERFLOW:
				printf("Stack Overflow!");
				break;
			case GL_STACK_UNDERFLOW:
				printf("Stack Underflow!");
				break;
			case GL_OUT_OF_MEMORY:
				printf("Out of memory!");
				break;
			default:
				break;
			}
		}
}
