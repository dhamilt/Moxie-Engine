#include "glPCH.h"
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Log.h"



void Log::Message(MessageTypes type, std::string message, ...)
{
	/*if (SDL_GL_GetCurrentContext() != glContext)
		SDL_GL_MakeCurrent(logWindow, glContext);
	
	if (imGuiContext != ImGui::GetCurrentContext())
		ImGui::SetCurrentContext(imGuiContext);

	SDL_GL_SwapWindow(logWindow);*/

	std::string  msg = "";
	if (type == MessageTypes::Error)
		msg += "LogError: ";
	else if (type == MessageTypes::Fatal)
		msg += "LogFatal: ";
	else if (type == MessageTypes::Message)
		msg += "LogMessage: ";
	else
		msg += "LogWarning: ";
	std::string result = msg + message;
	char buffer[125];

	
	
	// Start a new frame for the Log Window GUI to render
	/*ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Log Window");*/

	//ImGui::ShowDemoWindow();
	
	
	va_list args;
	va_start(args, result);
	vfprintf(stderr, result.c_str(), args/*, __FILE__, __FUNCTION__, __LINE__*/);
	va_end(args);

	result +=  " ";
	result += __FILE__;
	result += " ";
	result += __FUNCTION__;
	result += " ";
	result += std::to_string(__LINE__);
	result += "\n";

	logMessages.push_back(result);
	//ImGui::Text(result.c_str());
	
	//ImGui::End();
	//// Render the GUI
	//ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
}


void Log::Paint()
{
	// Change window focus
	if (SDL_GL_GetCurrentContext() != glContext)
		SDL_GL_MakeCurrent(logWindow, glContext);

	// Make sure the GUI is working in the Log window
	if (imGuiContext != ImGui::GetCurrentContext())
		ImGui::SetCurrentContext(imGuiContext);

	// What color to use when clearing the screen
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0f, 0.25f, 1.0f, 1.0f);// Cyan background
	
		
	
		
	// Start a new frame for the Log Window GUI to render
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	bool isOpen;
	
	ImGui::SetNextWindowPos(ImVec2());
	ImGui::Begin("Log Window", &isOpen, ImGuiWindowFlags_AlwaysVerticalScrollbar/*|ImGuiWindowFlags_AlwaysAutoResize*/);
	//ImVec2 windowPos = ImGui::GetWindowPos();
	int width, height;
	SDL_GetWindowSize(logWindow, &width, &height);
	ImGui::SetWindowSize("Log Window", ImVec2(width, height));
	for (int i = 0; i < logMessages.size(); i++)
		ImGui::Text(logMessages[i].c_str());

	ImGui::End();
	// Render the GUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	SDL_GL_SwapWindow(logWindow);

}

void Log::CreateWindow()
{
	// Create a Log window
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)( SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	logWindow = SDL_CreateWindow("Log Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, windowFlags);

	// Create a context for this window
	glContext = SDL_GL_CreateContext(logWindow);

	// Set this window as the current context
	SDL_GL_MakeCurrent(logWindow, glContext);
	
	// Setup GUI
	IMGUI_CHECKVERSION();

	// Create a new context for the log window
	imGuiContext = ImGui::CreateContext();

	// Make this the current context for the GUI
	if(ImGui::GetCurrentContext() != imGuiContext)
		ImGui::SetCurrentContext(imGuiContext);

	// Link the GUI to the correct context and rendering frameworks
	ImGui_ImplSDL2_InitForOpenGL(logWindow, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");


	// Set the Gui style
	ImGui::StyleColorsDark();
}
