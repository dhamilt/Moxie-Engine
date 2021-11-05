// OpenGLTutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include<GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 1280, HEIGHT = 720;

// Vertex Array Object, Vertex Buffer Object, and the shader itself
GLuint VAO, VBO, Shader;

// Create a Vertex Shader for the Triangle
static /*const*/ std::string vShader = "								\n\
#version 330 core													\n\
																	\n\
layout (location = 0) in vec3 pos;									\n\
																	\n\
void main()															\n\
{																	\n\
	gl_Position = vec4(pos.xyz * 0.5, 1.0);		\n\
}";

// Create a Fragment Shader for the Triangle
static /*const*/ std::string fShader = "								\n\
#version 330 core													\n\
																	\n\
out vec4 color;														\n\
																	\n\
void main()															\n\
{																	\n\
	color = vec4(0.15, 0.3, 0.6, 1.0);								\n\
}																	\n\
";

// Extracts vertex and fragment shaders from the shader code within a shader file
void ExtractShaders(std::string filepath);

// Adds the created shaders to the Shader Program
void AddShader(GLuint theProgram, const std::string& shaderCode, GLenum shaderType);

// Compiles the created shaders
void CompileShaders();


// Creates a triangle from defined vertices
void CreateTriangle();


int main()
{	
   /* std::cout << "Hello World!\n";*/
	// Initialize GLFW libary
	glfwInit();

	// If initialization of GLFW  failed
	if (glfwInit() == GLFW_FALSE)
	{
		// Display error message
		std::cout << "Window creation failed!" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Create a 1280 x 720 window with the title of 'My First Window'
	GLFWwindow* window = glfwCreateWindow(1280, 720, "My First Window", NULL, NULL);
	
	// If no window was created
	if (!window)
	{
		// Display error message
		std::cout << "Window creation failed!";
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

	// Set the context for GLEW to use
	glfwMakeContextCurrent(window);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	// if GLEW initialization fails
	if (glewInit() != GLEW_OK)
	{
		// Display error message
		std::cout << "GLEW initialization failed!" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}

	CreateTriangle();
	ExtractShaders("Resources/Basic.shader");
	CompileShaders();

	// Setup Viewport Size
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Loop until window closed
	while (!glfwWindowShouldClose(window))
	{
		// Get/Handle user input events
		glfwPollEvents();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Assign shader program
		glUseProgram(Shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Unassign shader program
		glUseProgram(0);

		

		// Swap buffers
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void CreateTriangle()
{
	GLfloat vertices[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Create a Vertex Array based on an id
	glGenVertexArrays(1, &VAO);

	//  Bind the VAO to an id
	glBindVertexArray(VAO);

	// Create a Vertex Buffer based on the same id
	glGenBuffers(1, &VBO);

	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Pass in the vertices to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an Attribute Pointer for the shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Enable the above Attribute Pointer
	glEnableVertexAttribArray(0);

	// Unbind the previous buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the previous vertex array
	glBindVertexArray(0);
}


void ExtractShaders(std::string filepath)
{
	std::ifstream in(filepath);

	std::string line = "";

	enum class ShaderType {none, vertex, fragment};
	ShaderType currentShaderType = ShaderType::none;
	if (!in.is_open())
		std::cout << "Error! Unable to read file at " << filepath << ". Path does not exist!" << std::endl;

	else
	{
		while (getline(in, line))
		{
			// if currently reading the vertex shader code
			if (line.find("#vertex") != std::string::npos)
			{
				currentShaderType = ShaderType::vertex;
				// clear vertex shader code
				vShader.clear();
			}
			// if currently reading the fragment shader code
			else if (line.find("#fragment") != std::string::npos)
			{
				currentShaderType = ShaderType::fragment;
				// clear fragment shader code
				fShader.clear();
			}
			// else if shader type is already defined
			else
			{
				// read shader code in code block
				switch (currentShaderType)
				{
				case ShaderType::none:
					break;
				case ShaderType::vertex:
					vShader.append(line + "\n");
					break;
				case ShaderType::fragment:
					fShader.append(line + "\n");
					break;
				default:
					break;
				}
			}
		}
	}
}

void AddShader(GLuint theProgram, const std::string& shaderCode, GLenum shaderType)
{
	// Creates a shader based on the parameter type
	GLuint theShader = glCreateShader(shaderType);

	// Pass in the shader code 
	const char* theCode = shaderCode.c_str();
	

	// Get the length of the shader file as a string
	int codeLength =static_cast<int>(shaderCode.length());
	//codeLength[0] = strlen(shaderCode);

	// Add the shader code to the shader 
	glShaderSource(theShader, 1, &theCode, &codeLength);

	// Compile the shader
	glCompileShader(theShader);

	// Get error message if one occurs
	GLint Result = 0;
	GLchar ErrorLog[1024] = { 0 };
	
	// Retrieve the compile status of the shader
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &Result);

	// If an error occured when compiling the shader
	if (Result == GL_FALSE)
	{
		// Log the error
		glGetShaderInfoLog(Shader, sizeof(ErrorLog), NULL, ErrorLog);

		// Print the error log
		std::cout << "Error compiling " << (shaderType == GL_VERTEX_SHADER ? "vertex " : "fragment")  << " shader: " << ErrorLog << "\nEnding shader compilation!" << std::endl;
		return;
	}

	// Else attach the shader to the shader program
	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	// Create a shader program
	Shader = glCreateProgram();

	// if null
	if (!Shader)
	{
		// Display error message
		std::cout << "Error creating shader program!" << std::endl;
		return;
	}

	// Add the Vertex and Fragment shaders to the shader program
	AddShader(Shader, vShader, GL_VERTEX_SHADER);
	AddShader(Shader, fShader, GL_FRAGMENT_SHADER);

	// Get error message if one occurs
	GLint Result = 0;
	GLchar ErrorLog[1024] = { 0 };

	// Link the shader program to the Graphics card
	glLinkProgram(Shader);

	// Retrieve the link status of the shader program
	glGetProgramiv(Shader, GL_LINK_STATUS, &Result);

	// If an error occured when linking the shader program
	if (Result == GL_FALSE)
	{
		// Log the error
		glGetProgramInfoLog(Shader, sizeof(ErrorLog), NULL, ErrorLog);
		
		// Print the error log
		std::cout << "Error linking program: " << ErrorLog << "\nEnding shader compilation!" << std::endl;
		return;
	}

	// Validate the shader program
	glValidateProgram(Shader);
	glGetProgramiv(Shader, GL_VALIDATE_STATUS, &Result);

	// If shader program was used improperly
	if (Result == GL_FALSE)
	{
		// Log the result
		glGetProgramInfoLog(Shader, sizeof(ErrorLog), NULL, ErrorLog);

		// Print the result
		std::cout << "Error validating program: " << ErrorLog << "\nEnding shader compilation!" << std::endl;
		return;
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
