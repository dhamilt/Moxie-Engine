#pragma once
#include "Log.h"
#include <functional>
#include "GameLoop.h"

extern GameLoop* GGLPtr;
#define  MLOG_MESSAGE(msg, ...)	{	if(GGLPtr->logWindow)GGLPtr->logWindow->Message(MessageTypes::Message, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);	}
#define  MLOG_WARNING(msg, ...)	{	if(GGLPtr->logWindow)GGLPtr->logWindow->Message(MessageTypes::Warning, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);	}
#define  MLOG_FATAL(msg, ...)	{	if(GGLPtr->logWindow)GGLPtr->logWindow->Message(MessageTypes::Fatal, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);	}
#define  MLOG_ERROR(msg, ...)	{	if(GGLPtr->logWindow)GGLPtr->logWindow->Message(MessageTypes::Error, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);	}