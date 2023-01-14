#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL.h>
#include "Graphic.h"
#include <vector>
#include "Camera.h"
#include "Light.h"
#include <unordered_map>
#include "MViewport.h"
#include "imgui/imgui_internal.h"
#include "MainMenu.h"
#include "RenderingPipeline.h"
class Material;


// TODO: Have the default rendering language be determined by an ini file
// for now, set the rendering language to be Vulkan

#define USE_RENDERING_VULKAN 1

#define USE_RENDERING_OPENGL 0

// TODO: IMPLEMENT A LIGHT STRUCT AND HAVE EVERY INITIALIZATION CALL
// FOR A LIGHT END UP ADDING TO A QUEUE OF LIGHTS TO BE DRAWN ON EVERY
// MESH IN THE SCENE
// Facade class used to hide the GLEW, GLFW, and callbacks
class GLSetup
{
	public:
		GLSetup();
		~GLSetup();
		void Init();
		ImGuiContext* GetCurrentContext();		
		bool IsViewportInFocus();
		void GetWindowDimensions(int& w, int& h);
		float GetFarClippingPlane();
		float GetNearClippingPlane();
		void SetFarClippingPlane(float val);
		void SetNearClippingPlane(float val);
		void GetViewportTextureID(GLuint& textureID, GLuint& renderbufferObjectID);
		void GetViewportDimensions(int& _width, int& _height);
		void AddMaterialToPipeline(std::string primitiveName, Material* mat);
		void AddCubemapMaterial(Material* cubeMapMat);
		void GetDefaultMeshShader(Shader* defaultShader);
		void SubmitCubeMapData(std::vector<TextureData*> cubemapData);
		void ImportMesh(std::string primitiveName, Mesh* mesh);
		void ImportMesh(std::string primitiveName, std::vector<DVertex> vertices, std::vector<uint16_t> indices);
		BRenderingPipeline* GetPipeline();
		DMat4x4 GetCameraView();
		DMat4x4 GetProjection();
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
	// Framebuffer ID
	GLuint screenTextureID;
	// Framebuffer Object and Renderbuffer Object
	GLuint fbo, rbo;
	// Viewport
	WViewport* viewport;
	int width = 800, height = 600;
	float nearClippingPlane = 0.1f;
	float farClippingPlane = 100.0f;
	SDL_Window* sdlWindow = nullptr;
	SDL_GLContext mainSDLContext;
	std::vector<Graphic*> renderObjs;
	std::vector<GUI_Base*> uiElements;
	int mouseWheelPos = 0;
	float fov = 60.0f;
	float fovMinConstraint = 20.0f;
	float fovMaxConstraint = 120.0f;
	DMat4x4 view = DMat4x4(1);
	DMat4x4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
	// Array cache of all light info in the level
	std::vector<Light> lightsCache;
	std::unordered_map<std::string, Light> lightMap;
	float white4[4]{1.0f, 1.0f, 1.0f, 1.0f};
	ImGuiContext* mainWindowGUIContext;
	ImPlotContext* mainImPlotContext;
	ImGuiWindow* windowInFocus;
	bool viewportInFocus;
	WMainMenu* mainMenu;
	BRenderingPipeline* pipeline;
	
};

/// <summary>
/// Global pointer to the render loop
/// </summary>
extern GLSetup* GGLSPtr;