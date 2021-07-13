#include "glPCH.h"

#include "GLSetup.h"
#include <cassert>
#include "GameLoop.h"


extern GLSetup* GGLSPtr = new GLSetup();
extern GameLoop* GGLPtr;

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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_SAMPLES, 4); // 4x anti-aliasing
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

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

	// Set the GUI style
	ImGui::StyleColorsDark();

	// Link the GUI to the correct context and rendering frameworks
	ImGui_ImplSDL2_InitForOpenGL(sdlWindow, mainSDLContext);
	ImGui_ImplOpenGL3_Init("#version 330");

	

	// Setup main camera
	mainCamera = new Camera(vector3(0, 1, 0));

	// Set up mouse capture callback for rotating the camera
	GGLPtr->AddMouseCapCallback(std::bind(&Camera::RotateCamera, mainCamera, std::placeholders::_1, std::placeholders::_2));
	GGLPtr->GetMainInputHandle()->leftMouseButton->Subscribe(std::bind(&Camera::OnMouseUp, mainCamera), MOX_RELEASED);

	// Set up mouse scroll wheel callback for changing the fov
	GGLPtr->GetMainInputHandle()->scrollWheel->Subscribe(std::bind(&GLSetup::ScrollWheelCallback, this, std::placeholders::_1));
	// Enable Depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragments that are closer to the camera
	glDepthFunc(GL_LESS);
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

		// What color to use when clearing the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// Black background
		
		// Start a new frame for the GUI to render
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// Load the GUI window
		windowLoaded = ImGui::Begin("GUI Window");

		// if the GUI window could not be loaded,
		// end the GUI window
		if (!windowLoaded)
			ImGui::End();

		// Keep a reference of the 4x4 view and projection matrices each frame
		// in order to pass into the drawing of meshes
		view = mainCamera->GetViewMatrix();
		
		projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
		
		// Send projection and view matrices to objects
		// being rendered
		for (int i = 0; i < renderObjs.size(); i++)
			renderObjs[i]->Draw(projection, view);

		/*ImGui::ShowDemoWindow();*/
		
		ImGui::ColorEdit4("My Color Picker", white4);

		if(windowLoaded)
			ImGui::End();
				
		// Render the GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		// Refresh screen with new buffer
		SDL_GL_SwapWindow(sdlWindow);
	}


	if (!gameLog)
		gameLog = &Log::Get();

	gameLog->Paint();
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

void GLSetup::operator+=(Graphic* ptr)
{
	renderObjs.push_back(ptr);
	printf("There are currently %d objects in the render queue.\n", (int)renderObjs.size());
}

void GLSetup::operator-=(Graphic* ptr)
{
	int index = 0;
	for (int i = 0; i < renderObjs.size(); i++)
	{
		if (*renderObjs[i] == *ptr)
			index = i;
	}
	if (index < 1)
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

