#include "glPCH.h"
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Log.h"



void Log::Message(MessageTypes type, const char* message, ...)
{	

	std::string  msg = "";
	if (type == MessageTypes::Error)
	{
		msg += "LogError: ";
		textColors.push_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (type == MessageTypes::Fatal)
	{
		msg += "LogFatal: ";
		textColors.push_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (type == MessageTypes::Message)
	{
		msg += "LogMessage: ";
		textColors.push_back(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		msg += "LogWarning: ";
		textColors.push_back(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	
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
	std::string msg = "";
	if (type == MessageTypes::Error)
	{
		msg += "LogError: ";
		textColors.push_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (type == MessageTypes::Fatal)
	{
		msg += "LogFatal: ";
		textColors.push_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (type == MessageTypes::Message)
	{
		msg += "LogMessage: ";
		textColors.push_back(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		msg += "LogWarning: ";
		textColors.push_back(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	}

	char buffer[1024];

	va_list args;
	va_start(args, message);
	vsprintf(buffer, message, args);
	va_end(args);

	msg += buffer;
	msg += " ";
	msg += fileName;
	msg += ": Line ";
	msg += std::to_string(line);
	msg += " ";
	msg += functionName;
	msg += "\n";

	logMessages.push_back(msg);
}


void Log::Paint()
{		
	ImGui::Begin("Log Window", &isWindowOpen);
	
	for (int i = 0; i < logMessages.size(); i++)
		ImGui::TextColored(textColors[i], logMessages[i].c_str());

	ImGui::End();
}

bool Log::operator==( GUI_Base& other)
{	
	return GetName() == other.GetName();
}

//void Log::CreateWindow()
//{
//	// Create a Log window
//	SDL_WindowFlags windowFlags = (SDL_WindowFlags)( SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
//	logWindow = SDL_CreateWindow("Log Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, windowFlags);
//
//	// Create a context for this window
//	glContext = SDL_GL_CreateContext(logWindow);
//
//	// Set this window as the current context
//	SDL_GL_MakeCurrent(logWindow, glContext);
//	
//	// Setup GUI
//	IMGUI_CHECKVERSION();
//
//	// Create a new context for the log window
//	imGuiContext = ImGui::CreateContext();
//
//	// Make this the current context for the GUI
//	if(ImGui::GetCurrentContext() != imGuiContext)
//		ImGui::SetCurrentContext(imGuiContext);
//
//	// Link the GUI to the correct context and rendering frameworks
//	ImGui_ImplSDL2_InitForOpenGL(logWindow, glContext);
//	ImGui_ImplOpenGL3_Init("#version 330");
//
//
//	// Set the Gui style
//	ImGui::StyleColorsDark();
//}
