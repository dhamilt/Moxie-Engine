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
#include "MViewport.h"

static const uint8_t MAX_POSTPROCESSING_LAYERS = 14;

// TODO: IMPLEMENT A LIGHT STRUCT AND HAVE EVERY INITIALIZATION CALL
// FOR A LIGHT END UP ADDING TO A QUEUE OF LIGHTS TO BE DRAWN ON EVERY
// MESH IN THE SCENE
// Facade class used to hide the GLEW, GLFW, and callbacks
class GLSetup
{
	public:
		GLSetup();
		~GLSetup();
		// Delayed Initialization function
		void Init();
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
		void GetViewport(std::vector<GLuint>& colorAttachments, uint8_t& numOfRenderingLayers, GLuint& renderBufferObjectID);
		void GetViewport(std::vector<GLuint>& colorAttachments, uint8_t& numOfRenderingLayers, GLuint& renderBufferObjectID, GLuint& defaultFramebuffer);
		void GetViewportDimensions(int& _width, int& _height);
		void SetViewportDimensions(const int& _width, const int& _height);
		void GetCurrentFramebuffer(GLuint& _fbo);
private:	
	void GLFW_ErrorCallback(int error, const char* description);
	void ScrollWheelCallback(int axisVal);
	// Generates the framebuffer, renderbuffer, initial rendering layer,
	// and all of the additional color attachments
	void FramebufferSetup();
	// Error Checking - TODO: Make namespace for the rendering part of the engine
	// and place these functions within in order to decouple the functions from this 
	// global rendering pipeline
	void GLErrorCheck(GLenum result);
	void RenderScreenQuad();

public:
	Camera* mainCamera;
	bool windowLoaded = false;


private:
	int width = 1600, height = 900;		
	SDL_Window* sdlWindow = nullptr;
	SDL_GLContext mainSDLContext;
	std::vector<Graphic*> renderObjs;
	std::vector<GUI_Base*> uiElements;
	GLuint currentFBO, rbo;
	std::vector<GLuint> renderingLayers;
	MViewport* viewport;
	int mouseWheelPos = 0;
	float fov = 60.0f;
	float fovMinConstraint = 20.0f;
	float fovMaxConstraint = 120.0f;
	mat4 view = mat4(1);
	mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
	// Array cache of all light info in the level
	std::vector<Light> lightsCache;
	std::unordered_map<std::string, Light> lightMap;
	float white4[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLuint screenQuadVAO, screenQuadVBO;
	ImGuiContext* mainWindowGUIContext;
	// Number of post processing layers to add to rendering out framebuffer
	uint8_t numOfPostProcessingLayers = 1;
	uint8_t numOfColorBuffers;
	// The current framebuffer/rendering pass that the rendering
	// pipeline is rendering out
	uint8_t currentRenderingPass = 0;
	// Total number of rendering passes that the pipeline is going to make each frame
	uint8_t renderingPassTotal = 3;

	// TODO: Create infrastructure to specify what type rendering
	// is being done before creating a framebuffer for it
	std::vector<GLuint> tempFBOs;
	std::vector<GLuint> tempRBOs;
};

/// <summary>
/// Global pointer to the render loop
/// </summary>
extern GLSetup* GGLSPtr;