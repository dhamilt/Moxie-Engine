#include "glPCH.h"
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Log.h"



void Log::Message(MessageTypes type, const char* message, ...)
{	
	MessageInfo msgInfo;
	msgInfo.messageType = type;
	std::string  result = "";
	if (type == MessageTypes::Error)
		result += "LogError: ";
	else if (type == MessageTypes::Fatal)
		result += "LogFatal: ";
	else if (type == MessageTypes::Message)
		result += "LogMessage: ";
	else
		result += "LogWarning: ";
	
	char buffer[1024];

	
	va_list args;
	va_start(args, message);
	vsprintf(buffer, message, args);
	va_end(args);
	
	result += buffer;
	result += "\n";

	msgInfo.message = result;
	logMessages.push_back(msgInfo);
	
}

void Log::Message(MessageTypes type, const char* message, const char* fileName, int line, const char* functionName, ...)
{
	MessageInfo msgInfo;
	msgInfo.messageType = type;
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

	
	msgInfo.message = result;
	logMessages.push_back(msgInfo);
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
	for (auto it = logMessages.begin(); it != logMessages.end(); it++)
	{
		if(it->messageType == MessageTypes::Message)
			ImGui::Text(it->message.c_str());
		else
		{
			if (it->messageType == MessageTypes::Warning)
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), it->message.c_str());
			else
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), it->message.c_str());
		}
		
	}

	ImGui::End();

}

bool Log::operator==( GUI_Base& other)
{	
	return GetName() == other.GetName();
}

