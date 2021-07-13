#pragma once
#include "glPCH.h"
#include <string>
#include <imgui.h>

enum class MessageTypes
{
	Message,
	Warning,
	Error,
	Fatal
};

class Log
{
public:
	
	static Log& Get()
	{
		static Log instance;
		instance.CreateWindow();
		instance.Message(MessageTypes::Message, "Window created.");
		return instance;
	}

	
	static MessageTypes MessageType;
	void Message(MessageTypes type, std::string message, ...);
	void Paint();

private:
	void CreateWindow();
	Log() { };
	Log(const Log&) {}
	void operator=(const Log&) {};
private:	
	 SDL_Window* logWindow = nullptr;
	 SDL_GLContext glContext = NULL;
	 ImGuiContext* imGuiContext = nullptr;
	 std::vector<std::string> logMessages;
	 
};

