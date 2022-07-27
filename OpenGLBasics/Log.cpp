#include "glPCH.h"
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Log.h"



void Log::Message(MessageTypes type, const char* message, ...)
{	

	std::string  msg = "";
	if (type == MessageTypes::Error)
		msg += "LogError: ";
	else if (type == MessageTypes::Fatal)
		msg += "LogFatal: ";
	else if (type == MessageTypes::Message)
		msg += "LogMessage: ";
	else
		msg += "LogWarning: ";
	
	char buffer[1024];

	
	va_list args;
	va_start(args, message);
	vsprintf(buffer, message, args);
	va_end(args);
	
	msg += buffer;
	msg += "\n";

	logMessages.push_back(msg);
	
}

void Log::Message(MessageTypes type, const char* message, const char* fileName, int line, const char* functionName, ...)
{
	std::string result = "";
	if (type == MessageTypes::Message)
		result = "LogMessage: ";
	else if (type == MessageTypes::Error)
		result = "LogError: ";
	else if (type == MessageTypes::Warning)
		result = "LogWarning: ";
	else
		result = "LogFatal: ";

	char buffer[1024];

	va_list args;
	va_start(args, message);
	vsprintf(buffer, message, args);
	va_end(args);

	result += buffer;
	result += " ";
	result += fileName;
	result += ": Line ";
	result += std::to_string(line);
	result += " ";
	result += functionName;
	result += "\n";

	logMessages.push_back(result);
}


void Log::Paint()
{		
	if (!ImGui::Begin("Log Window", &isWindowOpen))
	{
		// if the window couldn't be loaded
		// end the usage of this window
		ImGui::End();
		// leave the scope of this function
		return;
	}
	for (int i = 0; i < logMessages.size(); i++)
		ImGui::Text(logMessages[i].c_str());

	ImGui::End();

}

bool Log::operator==( GUI_Base& other)
{	
	return GetName() == other.GetName();
}

