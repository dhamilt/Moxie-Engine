#pragma once
#include "Log.h"
#include <functional>
#include "GameLoop.h"

extern GameLoop* GGLPtr;
#define  MLOG_MESSAGE(msg, ...){if(GGLPtr)GGLPtr->logWindow->Message(MessageTypes::Message, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);} /*{std::function<void>(Log::Message, &GGLPtr->logWindow) Log::Get().Message(MessageTypes::Message, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}*/
#define  MLOG_WARNING(msg, ...){Log::Get().Message(MessageTypes::Warning, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}
#define  MLOG_FATAL(msg, ...){Log::Get().Message(MessageTypes::Fatal, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}
#define  MLOG_ERROR(msg, ...){Log::Get().Message(MessageTypes::Error, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}