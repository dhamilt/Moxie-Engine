#include "glPCH.h"
#include "GLSetup.h"
#include <cassert>
#include "GameLoop.h"
#include "FramebufferParams.h"

extern GLSetup* GGLSPtr = new GLSetup();
extern GameLoop* GGLPtr;

// the max number of framebuffers that can be used for multi-pass rendering
static const uint8_t MAX_FRAMEBUFFER_COUNT = 32;
GLSetup::GLSetup()
{
	bool result = glfwInit();
	if (!result)
	{
		perror("Unable to initialize GLFW!");
		assert(result);
		throw std::runtime_error("Rendering framework could not be initialized!");
	}

	StartSDLWindow();

}

GLSetup::~GLSetup()
{
	// Stop rendering the depth and stencil rendering
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_STENCIL_TEST);

	// destroy the viewport
	if (viewport)
		delete viewport;

	// destroy the camera
	if (mainCamera)
		delete mainCamera;

	// Remove all color attachment textures
	glDeleteTextures(numOfColorBuffers * renderingPassTotal, &renderingLayers[0]);	

	// Remove all framebuffers
	glDeleteFramebuffers(renderingPassTotal, &tempFBOs[0]);
	// Remove renderbuffer(s)
	glDeleteRenderbuffers(1, &tempRBOs[0]);

	// Shutdown and clean GUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// if window exists destroy it
	if (sdlWindow)
		SDL_DestroyWindow(sdlWindow);
	
	// End usage of GLFW
	glfwTerminate();
}



void GLSetup::StartSDLWindow()
{
	// Set the minimum OpenGL version this program run on
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	
	

	// Cull backfacing polygons
	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);*/
	

	// Creates a window using SDL
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	sdlWindow = SDL_CreateWindow("Moxie Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);



	if (!sdlWindow)
	{
		printf("Error! %s\n", SDL_GetError());
		assert(sdlWindow);
		throw std::runtime_error("Context window could not be created!");
	}

	// Create an OpenGL context associated with the SDL window
	mainSDLContext = SDL_GL_CreateContext(sdlWindow);

	// Make the window the current context 
	SDL_GL_MakeCurrent(sdlWindow, mainSDLContext);	

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	bool glewResult = glewInit();
	if (glewResult == GLEW_OK)
	{
		// Lower the number of buffer swaps for each frame
		// in order to optimize efficiency(V-Sync)
		SDL_GL_SetSwapInterval(1);

		//glfwGetFramebufferSize(window, &width, &height);
		//glViewport(0, 0, width, height);
		// Get the pixel dimensions for the framebuffer
		SDL_GL_GetDrawableSize(sdlWindow, &width, &height);
	}
	// otherwise throw an exception
	else
	{
		assert(glewResult == GLEW_OK);
		throw std::runtime_error("GLEW could not be initialized!");
	}
	// Setup GUI
	IMGUI_CHECKVERSION();
	mainWindowGUIContext = ImGui::CreateContext();	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Set the GUI style
	ImGui::StyleColorsDark();

	std::string renderingVersion = "#version 330";
	// Link the GUI to the correct context and rendering frameworks
	ImGui_ImplSDL2_InitForOpenGL(sdlWindow, mainSDLContext);
	ImGui_ImplOpenGL3_Init(&renderingVersion[0]);
	// Enable Depth and Stencil rendering
	/*glEnable(GL_DEPTH_TEST);*/
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	/*glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);*/
	// Accept fragments that are closer to the camera
	
	// Setup framebuffer and all of it's layered attachments
	FramebufferSetup();
	// Setup main camera
	mainCamera = new Camera(vector3(0, 1, 0));

	// Set up mouse capture callback for rotating the camera
	GGLPtr->AddMouseCapCallback(std::bind(&Camera::RotateCamera, mainCamera, std::placeholders::_1, std::placeholders::_2));
	GGLPtr->GetMainInputHandle()->leftMouseButton->Subscribe(std::bind(&Camera::OnMouseUp, mainCamera), MOX_RELEASED);

	// Set up mouse scroll wheel callback for changing the fov
	GGLPtr->GetMainInputHandle()->scrollWheel->Subscribe(std::bind(&GLSetup::ScrollWheelCallback, this, std::placeholders::_1));
	
}

void GLSetup::UpdateLightingCollection(std::string lightComponentName, Light* lightInfo)
{
	// if the current light has not been added to the light map
	if (lightMap.find(lightComponentName) == lightMap.end())
	{
		// add it to the queue
		lightMap.insert(std::pair<std::string, Light>(lightComponentName,*lightInfo));
	}
	// otherwise update the light info
	else
		lightMap[lightComponentName] = *lightInfo;

	// Update the light info array cache
	// TODO: Refactor this light caching to be event driven
	lightsCache.clear();
	for (std::pair<std::string, Light> lightInfo : lightMap)
		lightsCache.push_back(lightInfo.second);
}

void GLSetup::GetAllLightInfo(std::vector<Light>& _lightingInfo)
{
	_lightingInfo.clear();
	// replace and/or retrieve the current light info in the level
	_lightingInfo = lightsCache;
}

void GLSetup::GetViewport(std::vector<GLuint>& colorAttachments, uint8_t& numOfRenderingLayers, GLuint& renderBufferObjectID)
{
	colorAttachments = renderingLayers;
	renderBufferObjectID = rbo;
	numOfRenderingLayers = numOfPostProcessingLayers;
}

void GLSetup::GetViewport(std::vector<GLuint>& colorAttachments, uint8_t& numOfRenderingLayers, GLuint& renderBufferObjectID, GLuint& defaultFramebuffer)
{
	colorAttachments = renderingLayers;
	renderBufferObjectID = rbo;
	numOfRenderingLayers = numOfPostProcessingLayers;
	defaultFramebuffer = tempFBOs[(int)(tempFBOs.size() -1)];
}

void GLSetup::GetViewportDimensions( int& _width,  int& _height)
{
	_width = width;
	_height = height;
}

void GLSetup::SetViewportDimensions(const int& _width, const int& _height)
{
	width = _width;
	height = _height;
	glDeleteTextures(numOfColorBuffers * renderingPassTotal, &renderingLayers[0]);
	
	glGenTextures(numOfColorBuffers * renderingPassTotal, &renderingLayers[0]);

	glDeleteRenderbuffers(renderingPassTotal, &tempRBOs[0]);
	glGenRenderbuffers(renderingPassTotal, &tempRBOs[0]);
	// resize all of the color attachments for each of the framebuffers
	for (uint8_t i = 0; i < tempFBOs.size(); i++)
	{	
		glBindFramebuffer(GL_FRAMEBUFFER, tempFBOs[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, tempRBOs[i]);
		for (uint8_t j = 0; j < numOfColorBuffers; j++)
		{
			glBindTexture(GL_TEXTURE_2D, renderingLayers[i * numOfColorBuffers + j]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
			// Set texture params
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glGenerateMipmap(GL_TEXTURE_2D);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + j, GL_TEXTURE_2D, renderingLayers[i * numOfColorBuffers + j], 0);
		}
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tempRBOs[i]);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// ensure the opengl viewport is properly sized
	glViewport(0, 0, width, height);
}

void GLSetup::GetCurrentFramebuffer(GLuint& _fbo)
{
	_fbo = currentFBO;
}



void GLSetup::Render()
{
	// Run one frame of the Render thread
	if (sdlWindow)
	{
		// Make sure SDL working in the context of this window
		if (mainSDLContext != SDL_GL_GetCurrentContext())
			SDL_GL_MakeCurrent(sdlWindow, mainSDLContext);

		// Make sure the GUI is working in the main window
		if (mainWindowGUIContext != ImGui::GetCurrentContext())
			ImGui::SetCurrentContext(mainWindowGUIContext);
		
		
		
		// Keep a reference of the 4x4 view and projection matrices each frame
		// in order to pass into the drawing of meshes
		view = mainCamera->GetViewMatrix();

		projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.2f, 100.0f);

		for (uint8_t j = 0; j < renderingPassTotal; j++)
		{
			
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, tempFBOs[j]);
			// if on the second rendering pass or later
			if (j > 0)
			{
				//pass in the frame data from the previous framebuffer into the current framebuffer				
				glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFBOs[j-1]);

				// draw to the initial framebuffer or over previous framebuffer data
				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
			}
			
			
			// check to see if current framebuffer is complete
			GLuint temp = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			assert(temp == GL_FRAMEBUFFER_COMPLETE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
			// What color to use when clearing the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Black background

			// Send projection and view matrices to objects
			// being rendered
			for (int k = 0; k < renderObjs.size(); k++)
			{					
				// Tell the render objects which render pass is being currently rendered
				renderObjs[k]->SetRenderingPass(j);
				renderObjs[k]->Draw(projection, view);
			}
		}
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		// rebind to default framebuffer
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Start a new frame for the GUI to render
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		// Create a Dockable space for the Editor
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		// Call all Paint calls for UI elements 
		// that exist on the GUI
		for (int l = 0; l < uiElements.size(); l++)
			uiElements[l]->Paint();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		// Render the GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		// Refresh screen with new buffer
		SDL_GL_SwapWindow(sdlWindow);

		
	}
	
}

void GLSetup::Init()
{
	viewport = new MViewport();
}

void GLSetup::GetWindowDimensions(int& w, int& h)
{
	w = width;
	h = height;
}

mat4 GLSetup::GetCameraView()
{
	return view;
}

mat4 GLSetup::GetProjection()
{
	return projection;
}

void GLSetup::AddRenderObject(Graphic* ptr)
{
	renderObjs.push_back(ptr);
	printf("There are currently %d objects in the render queue.\n", (int)renderObjs.size());
}

// Remove render object if found in queue
void GLSetup::RemoveRenderObject(Graphic* ptr)
{
	int index = -1;
	int i = 0;
	while (i < renderObjs.size())
	{
		
		if (ptr->id == renderObjs[i]->id && ptr->type == renderObjs[i]->type)
		{
			index = i;
			break;
		}
		i++;
	}

	if (index >= 0)
		renderObjs.erase(renderObjs.begin() + index);
}

void GLSetup::GLFW_ErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void GLSetup::ScrollWheelCallback(int axisVal)
{
	if(fov + axisVal >= fovMinConstraint && fov + axisVal <= fovMaxConstraint)
	fov += axisVal;
}

void GLSetup::FramebufferSetup()
{
	// Generate the framebuffer(s)
	tempFBOs.resize(renderingPassTotal);
	tempRBOs.resize(renderingPassTotal);
	glGenFramebuffers(renderingPassTotal, &tempFBOs[0]);
	// Generate the renderbuffer
	glGenRenderbuffers(renderingPassTotal, &tempRBOs[0]);
		
	// Reserve memory for a texture(s)
	// based on how many rendering layers/color
	// attachments being rendered
	numOfColorBuffers = numOfPostProcessingLayers + 1;
	renderingLayers.resize((numOfColorBuffers) * renderingPassTotal);
	glGenTextures(numOfColorBuffers* renderingPassTotal, &renderingLayers[0]);
	std::vector<GLuint> colorAttachments;
	for (uint8_t k = 0; k < numOfColorBuffers; k++)
		colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + k);

	for (int j = 0; j < renderingPassTotal; j++)
	{
		// Bind the framebuffer for current render pass
		glBindFramebuffer(GL_FRAMEBUFFER, tempFBOs[j]);

		

		
		// Attach color attachment for each texture
		for (int i = 0; i < numOfColorBuffers; i++)
		{
			glBindTexture(GL_TEXTURE_2D, renderingLayers[j * numOfColorBuffers + i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
			// Set texture params
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Attach texture to the drawing and reading operations for the framebuffer
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderingLayers[j * numOfColorBuffers + i], 0);			
		}		
					
		// Bind the renderbuffer for current render pass
		glBindRenderbuffer(GL_RENDERBUFFER, tempRBOs[j]);
		// Store the depth and stencil data into the render buffer
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tempRBOs[j]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		

		// Tell OpenGL to use the correct number for color attachments		
		glDrawBuffers(numOfColorBuffers, &colorAttachments[0]);

		Moxie::GLErrorCheck(glGetError());

		// Check to see if each framebuffer created is valid
		GLuint fbResult = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fbResult != GL_FRAMEBUFFER_COMPLETE)
			MLOG_FATAL("Framebuffer not complete!");
		// throw an assert error if the framebuffer wasn't properly setup
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}
	
	

	
	
	Moxie::GLErrorCheck(glGetError());
}

void GLSetup::GLErrorCheck(GLenum result)
{
	switch (result)
	{
	case GL_NO_ERROR:
		printf("No error found.\n");
		break;

	case GL_INVALID_ENUM:
		perror("Invalid enum!");
		break;

	case GL_INVALID_VALUE:
		perror("Invalid value!");
		break;

	case GL_INVALID_OPERATION:
		perror("Invalid operation!");
		break;

	case GL_INVALID_FRAMEBUFFER_OPERATION:
		perror("Invalid framebuffer operation!");
		break;

	case GL_OUT_OF_MEMORY:
		perror("Out of memory!");
		break;

	case GL_STACK_UNDERFLOW:
		perror("Stack Underflow!");
		break;

	case GL_STACK_OVERFLOW:
		perror("Stack Overflow!");
		break;

	default:
		printf("No error found.\n");
		break;
	}
}

void GLSetup::RenderScreenQuad()
{
	if (!screenQuadVAO)
	{
		// vertex data for quad
		float quadVertices[] =
		{
			// positions       // texCoords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f
		};
		// setup vertex array object for quad
		glGenVertexArrays(1, &screenQuadVAO);
		// as well as the vertex buffer
		glGenBuffers(1, &screenQuadVBO);
		glBindVertexArray(screenQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(screenQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void GLSetup::AddUIElement(GUI_Base* ptr)
{
	uiElements.push_back(ptr);
}

void GLSetup::RemoveUIElement(GUI_Base* ptr)
{
	int i = 0;
	int index = -1;
	while (i < uiElements.size())
	{
		if (ptr->GetName() == uiElements[i]->GetName() && ptr->id == uiElements[i]->id)
		{
			index = i;
			break;
		}
		i++;
	}
	if (index >= 0)
		uiElements.erase(uiElements.begin() + index);
}

