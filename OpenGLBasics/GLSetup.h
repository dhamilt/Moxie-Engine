#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL.h>
#include "Graphic.h"
#include <vector>
#include "Camera.h"
#include "Light.h"
#include <unordered_map>
#include "GUI_Base.h"


// TODO: IMPLEMENT A LIGHT STRUCT AND HAVE EVERY INITIALIZATION CALL
// FOR A LIGHT END UP ADDING TO A QUEUE OF LIGHTS TO BE DRAWN ON EVERY
// MESH IN THE SCENE
// Facade class used to hide the GLEW, GLFW, and callbacks
class GLSetup
{
	public:
		GLSetup();
		~GLSetup();		
		void GetWindowDimensions(int& w, int& h);
		mat4 GetCameraView();
		mat4 GetProjection();
		void Render();
		void AddRenderObject(Graphic* ptr);
		void RemoveRenderObject(Graphic* ptr);
		void AddUIElement(GUI_Base* ptr);
		void RemoveUIElement(GUI_Base* ptr);
		void StartSDLWindow();		
		void UpdateLightingCollection(std::string lightComponentName, Light* lightInfo);
		void GetAllLightInfo(std::vector<Light>& _lightingInfo);
private:	
	void GLFW_ErrorCallback(int error, const char* description);
	void ScrollWheelCallback(int axisVal);
	

public:
	Camera* mainCamera;
	bool windowLoaded = false;


private:
	int width = 1600, height = 900;		
	SDL_Window* sdlWindow = nullptr;
	SDL_GLContext mainSDLContext;
	std::vector<Graphic*> renderObjs;
	std::vector<GUI_Base*> uiElements;
	int mouseWheelPos = 0;
	float fov = 60.0f;
	float fovMinConstraint = 20.0f;
	float fovMaxConstraint = 120.0f;
	mat4 view = mat4(1);
	mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
	// Array cache of all light info in the level
	std::vector<Light> lightsCache;
	std::unordered_map<std::string, Light> lightMap;
	float white4[4]{White.r, White.g, White.b, White.a};
	ImGuiContext* mainWindowGUIContext;


};

/// <summary>
/// Global pointer to the render loop
/// </summary>
extern GLSetup* GGLSPtr;