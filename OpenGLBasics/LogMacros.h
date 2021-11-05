#pragma once
#include "Log.h"
#include <functional>
#include "GameLoop.h"

extern GameLoop* GGLPtr;
#define  MLOG_MESSAGE(msg, ...){GGLPtr->SendLogMessage(MessageTypes::Message, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}
#define  MLOG_WARNING(msg, ...){GGLPtr->SendLogMessage(MessageTypes::Warning, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}
#define  MLOG_FATAL(msg, ...){GGLPtr->SendLogMessage(MessageTypes::Fatal, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}
#define  MLOG_ERROR(msg, ...){GGLPtr->SendLogMessage(MessageTypes::Error, msg, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);}