#pragma once
#include "glPCH.h"
#include <iostream>
#include "GLSetup.h"
#include "GameLoop.h"
#include "ImageLibrary.h"
#include "Cubemaps.h"
#include <cstdio>
#include <cassert>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <map>
#include <unordered_map>
#include "MActor.h"
#include "MeshComponent.h"
#include "MyLightComponent.h"
#include "RotatingLightComponent.h"
#include "Camera.h"
#include "TransformationLibrary.h"
#include "SpriteRenderer.h"
#include "LineShaperComponent.h"
#include "ShapeTransformationLibrary.h"
#include "PxPhysicsAPI.h"
#include "AudioEQComponent.h"

#define  MAXCHAR 1024

const float PI = 3.1415f;

uint8_t GetShaderFromFile(const char* filepath, char** shader);
GLuint LoadShaders(const char* v_filepath, const char* f_filepath);
bool LoadModelFromFile(const char* path, std::vector<vertex>& vertices, std::vector<uint16_t>&indices);
//void ProcessNode(aiNode* node, const aiScene scene);
void ProcessMesh(const aiScene*, std::vector<vertex>&vertices, std::vector<uint16_t>&indices);
GLuint LoadBMP(const char* imgPath);
bool LoadObjC(const char* filePath, std::vector<vertex>& vertices, std::vector<GLushort>& indices);
bool LoadObj(const char* filePath, std::vector<vertex>& vertices, std::vector<unsigned short>& indices);
mat4 GetProjectionMatrix(const float& _fov, const float& aspectRatio);
mat4 GetViewMatrix(const vector3& pos, const vector3& dir, const vector3 up);
void GLErrorHandling(GLenum errorType)
{
	
	switch (errorType)
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

extern GLSetup* GGLSPtr;
extern GameLoop* GGLPtr;

int main(int argc, char* argv[])
{	
	// Delayed initialization("Lazy" init)
	GGLPtr->Init();
	GGLSPtr->Init();
	/*ALCdevice* device = alcOpenDevice(NULL);
	ALCcontext* context = alcCreateContext(device, NULL);*/
	MSoundDataAsset* sound2 = new MSoundDataAsset("../Aritus - Summer With You.wav");
	MSoundDataAsset* sound = new MSoundDataAsset("../Large Professor - Frantic Barz (NIKK BLVKK Remix).wav");
	AudioSource* source = new AudioSource(/*device, context,*/ sound);
	//AudioSource* source2 = new AudioSource(device, context, sound);
	//source->SetSound(sound);
	//source2->SetSound(sound2);
	//source->Play();
	int i = 0;
	int maxPlayTimeInSeconds = 120;
	double time = 0.0f;
		

	// Add a skybox to the scene
	std::vector<std::string> faceFilePaths = { "../cubemaps/Yokohama3/posx.jpg",
											"../cubemaps/Yokohama3/negx.jpg",
											"../cubemaps/Yokohama3/posy.jpg",  
											"../cubemaps/Yokohama3/negy.jpg",
											"../cubemaps/Yokohama3/posz.jpg",
											"../cubemaps/Yokohama3/negz.jpg" };

	Cubemaps* skybox = new Cubemaps(faceFilePaths);

	GLErrorHandling(glGetError());


	if (skybox->IsValid())
		skybox->GetShaderFromPath("CubeMap.vertex", "CubeMap.fragment");

	// Load a OBJ file to use as a 3d model
	std::vector<vertex> vertices;
	std::vector<uint16_t> indices;
	
	bool modelLoaded = LoadObj("Suzanne.obj", vertices, indices);
	if (!modelLoaded)
		return 0;

	// Create an actor to exist in the scene
	MActor* actor = new MActor();

	// Create a texture
	Texture* texture = new Texture("MemeJoe.bmp");

	// Make a monkey mesh 
	MeshComponent* monkeyMeshComponent = new MeshComponent();
	monkeyMeshComponent->mesh = new Mesh(vertices, indices, "Lighting.vertex", "Lighting.fragment");
	monkeyMeshComponent->mesh->ApplyTexture(texture);
	monkeyMeshComponent->transform.Translate(vector3(0, 1, 7));
	monkeyMeshComponent->transform.SetRotation(quaternion(0.0f, vector3(0.0f, 1.0f, 0.0f)));
	actor->AddComponent(monkeyMeshComponent);

	// Make a light 
	MyLightComponent* lightComponent = new MyLightComponent();		
	lightComponent->transform.Rotate(vector3(0.0f, 1.0f, 0.0f), 90);
	lightComponent->transform.Translate(vector3(3, 1, 7));
	lightComponent->intensity = 2.0f;
	actor->AddComponent(lightComponent);

	MyLightComponent* secondLight = new MyLightComponent();
	secondLight->transform.Rotate(vector3(0.0f, 1.0f, 0.0f), -90);
	secondLight->transform.Translate(vector3(-3, 1, 7));
	secondLight->intensity = 2.0f;
	actor->AddComponent(secondLight);

	

	// Make a rotating light
	RotatingLightComponent* rotLight = new RotatingLightComponent();
	rotLight->SetTarget(&monkeyMeshComponent->transform);
	// set it to a spot light
	rotLight->lightType = LightComponent::LightTypes::Spot;
	// increase its radius
	rotLight->radius = 5.0f;
	rotLight->transform.Rotate(vector3(0.0f, 1.0f, 0.0f), -90);
	rotLight->transform.Translate(vector3(-3, 1, 7));
	rotLight->intensity = 60.0f;
	actor->AddComponent(rotLight);

	
	// Create an Audio Equalizer LineRendering component
	AudioEQComponent* lineEQ = new AudioEQComponent(vector3(-5.0f, 2.5f, 6.0f), vector3(5.0f, 2.5f, 6.0f));
	// Add an audio source to the component
	lineEQ->AddSource(source);
	// Change the EQ's max height variance
	lineEQ->ChangeMaxHeightVariance(1.25f);
	// Add a few interpolation points between samples
	//lineEQ->AddInterpolatePoints(2);
	// Change the number of samples it's capturing
	lineEQ->ChangeSampleCount(1024);
	// Change the volume of the component
	lineEQ->GetSource()->SetVolume(.5f);
	// Change the color of the component
	lineEQ->AddLerpColors(Cyan, Green);
	lineEQ->renderer->SetLineWidth(2.f);
	// then play the audio source
	lineEQ->PlaySource(true);
	// Add the component to an actor
	actor->AddComponent(lineEQ);
	//// Stay in the loop while the song from the component is playing
	//while (lineEQ->IsPlaying())
	//{
	//	SDL_Delay(100);
	//}

	//// Create a LineRenderComponent
	//LineShaperComponent* lineShaperComponent = new LineShaperComponent();
	//LineShaperComponent* secondShaper = new LineShaperComponent();
	//secondShaper->SetShape(Shapes::MaurerRose);
	//secondShaper->SetTimeOffset(-0.475f);
	//
	//
	//std::vector<vector3>linePoints;
	//
	//float maxRange = 10.0f;
	//int maxNumOfLinePoints = 120;

	//for (int i = 0; i < maxNumOfLinePoints; i++)
	//{
	//	float horizontalVal = 0.0f;
	//	horizontalVal = -maxRange + (float)i / (float)maxNumOfLinePoints * (maxRange - (-maxRange));
	//	

	//	linePoints.push_back(vector3(horizontalVal, 1.0f, 3.0f));
	//}
	//
	//lineShaperComponent->renderer->SetLineColor(Red);	
	//secondShaper->renderer->SetLineColor(White);
	//// Set Line width
	//lineShaperComponent->SetLineWidth(5.0f);
	//secondShaper->SetLineWidth(5.0f);
	//lineShaperComponent->SetScale(5.0f);
	//secondShaper->SetScale(5.0f);
	//lineShaperComponent->SetRevolutionCount(10.0f);
	//secondShaper->SetRevolutionCount(10.0f);
	//lineShaperComponent->SetTimeScale(10.0f);
	//secondShaper->SetTimeScale(50.f);
	//LineRenderer* lineRenderer = lineShaperComponent->renderer;
	//LineRenderer* secondRenderer = secondShaper->renderer;
	//lineRenderer->AddPoints(linePoints);	
	//actor->AddComponent(lineShaperComponent);
	//actor->AddComponent(secondShaper);
	//lineShaperComponent->transform.Translate(vector3(0, 3, 6));
	//secondShaper->transform.Translate(vector3(0, 3, 6));
	//// Print the forward vector of the light component	
	//quaternion lightRotation = lightComponent->GetWorldRotation();
	//vector3 forward = lightRotation* vector3(0, 0, 1);
	//printf("%1.2f, %1.2f, %1.2f", forward.x, forward.y, forward.z);
	
	actor->transform.SetPosition(vector3(0, -2, 0));
	GGLPtr->Loop();
	
	

	return 0;
}



uint8_t GetShaderFromFile(const char* filepath, char** shader)
{

	std::stringstream ss(std::ios::out | std::ios::binary);
	std::string line;
	std::ifstream ifs(filepath);

	while (std::getline(ifs, line)) ss << line << "\r\n";
	*shader = (char*)malloc(ss.str().length() + 1);
	memcpy(*shader, ss.str().c_str(), ss.str().length());
	*(*shader + ss.str().length()) = '\0';
	return 1;
	
}


// within this function twice. Should return the pointer to the
// file as a parameter on success and null on failure

GLuint LoadShaders(const char* v_filepath, const char* f_filepath)
{
	char* v_shader;
	char* f_shader;
	int infoLogLength = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLint result = GL_FALSE;
	if (GetShaderFromFile(v_filepath, &v_shader))
	{
		// Create Vertex Shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		printf("Compiling shader: %s\n", v_filepath);		
		glShaderSource(vertexShader, 1, &v_shader, NULL);
		glCompileShader(vertexShader);

		// Check Vertex Shader
		result = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLogLength+1);
			glGetShaderInfoLog(vertexShader, infoLogLength, NULL, infoLog);
			printf("%s\n", infoLog);
			free(infoLog);
		}
		if (!result)
			return result;
	}
	else
	{
		perror("Error opening vertex shader file!");
		return GL_FALSE;
	}

	if (GetShaderFromFile(f_filepath, &f_shader))
	{
		// Create Fragment Shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		printf("Compiling shader: %s\n", f_filepath);
		glShaderSource(fragmentShader, 1, &f_shader, NULL);
		glCompileShader(fragmentShader);

		// Check Fragment Shader
		result = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLogLength+1);
			glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, infoLog);
			printf("%s\n", infoLog);
			free(infoLog);
		}
		if (!result)
			return result;
	}
	else
	{
		perror("Error opening fragment shader file!");
		return GL_FALSE;
	}

	// Link the program
	printf("Linking program\n");
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Check status of program
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		char* infoLog = (char*)malloc(sizeof(char) * infoLogLength+1);
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
		printf("%s\n", infoLog);
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

bool LoadModelFromFile(const char* path, std::vector<vertex>& vertices, std::vector<uint16_t>& indices)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (scene)
	{
		ProcessMesh(scene, vertices, indices);
		return true;
	}

	perror("Unable to load model from file!");
	return false;
}

//void ProcessNode(aiNode* node, const aiScene scene,)
//{
//	//// process all of the nodes in the mesh (if any)
//	//for (uint32_t i = 0; i < node->mNumMeshes; i++)
//	//{
//	//	aiMesh* mesh = scene.mMeshes[node->mMeshes[i]];
//	//	
//	//}
//
//	
//}

void ProcessMesh(const aiScene* scene, std::vector<vertex>& vertices, std::vector<uint16_t>& indices)
{
	if (scene)
	{
		aiMesh* mesh = scene->mMeshes[0];
		unsigned int numOfUVs = mesh->GetNumUVChannels();
		// retrieve the vertex attributes
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			vertex tempVert;
			if (mesh->HasTextureCoords(i))
				tempVert = vertex(mesh->mVertices[i], *mesh->mTextureCoords[i], mesh->mNormals[i]);
			else
				tempVert = vertex(mesh->mVertices[i], mesh->mNormals[i]);

			vertices.push_back(tempVert);
		}

		// retrieve the index buffer for the mesh
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back((uint16_t)face.mIndices[j]);
			
		}
	}
	
}

GLuint LoadBMP(const char* imgPath)
{
	FILE* file = fopen(imgPath, "rb");
	if (file)
	{
		// the header of a BMP file is always exactly 54 bytes in size
		uint8_t header[54];
		// Position of the file where the data begins
		uint32_t dataPos;
		uint32_t wth, height;
		// The size of the image
		// The number of pixels (wth x height) multiplied by the number of color channels (3)
		uint32_t imageSize;
		// The actual texture data
		uint8_t* data;

		if (fread(header, sizeof(uint8_t), 54, file) != 54)
		{
			perror("Incorrect BMP file format!\n");
			return GL_FALSE;
		}

		if (header[0] != 'B' || header[1] != 'M')
		{
			perror("Incorrect BMP file format!\n");
			return GL_FALSE;
		}

		// read the ints from the byte array
		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		wth = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		// Some BMP files can be misformatted,
		// so fill with potential missing info
		imageSize = imageSize == 0 ? wth * height * 3 : imageSize;
		dataPos = dataPos == 0 ? 54 : dataPos;

		// Create data buffer
		data = (uint8_t*)calloc(imageSize, sizeof(uint8_t));
		
		if(data != 0)
			// Fill the buffer
			fread(data, sizeof(uint8_t), imageSize, file);

		// Create an OpenGL texture
		GLuint texture;
		glGenTextures(1, &texture);

		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// Feed the texture an image to render
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wth, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
		
		// When magnifying the texture (because there are no mipmaps at this size), use linear filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// When shrinking, the texture, use a linear blend of the two mipmaps it's closest to; each filtered linearly as well
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		

		glGenerateMipmap(GL_TEXTURE_2D);

		free(data);
		fclose(file);
		return GL_TRUE;
	}

	perror("Image could not be loaded from path!\n");
	return GL_FALSE;
}

bool LoadObjC(const char* filePath, std::vector<vertex>& vertices, std::vector<GLushort>& indices)
{
	FILE* file = fopen(filePath, "rb");
	if (file)
	{
		std::vector<uint32_t> vertexIndices, uvIndices, normalIndices;
		std::vector<vector3> temp_vertices;
		std::vector<vector2> temp_uvs;
		std::vector<vector3> temp_normals;
		std::vector<std::string> uniqueVertStrings;
		std::map<std::string, int> vertexToIndex;		
		
		int vertexIndex = 0;
		int textureCoordsUsed = 0;
		int lineIndex = 0;
		int fileLength = 0; // the number of lines in the file
		char lineHeader[256];
		
		while (fgets(lineHeader, sizeof(lineHeader), file))
		{
			fileLength++;
		}

		rewind(file);
		memset(lineHeader, '\0', sizeof(lineHeader));
		while (fgets(lineHeader, sizeof(lineHeader), file))
		{
			
 			if(lineHeader[0] != '#')
			{
				printf("\rReading file... %d%%", int((float)lineIndex / fileLength * 100));
				// read the current line				
				char* token = strtok(lineHeader, " ");
				// if line has vertex info
				if (strcmp(token,"v") == 0)
				{
					vector3 vertPos;
					if(sscanf(lineHeader+2, "%f %f %f\n", &vertPos.x, &vertPos.y, &vertPos.z) ==3)
						temp_vertices.push_back(vertPos);
				}

				// if line has texture coordinate info
				else if (strcmp(token, "vt") == 0)
				{
					if (!textureCoordsUsed)
						textureCoordsUsed = 1;
					vector2 uv;
					if(sscanf(lineHeader+3, "%f %f\n", &uv.x, &uv.y) == 3)
					temp_uvs.push_back(uv);
				}

				// if line has normal info
				else if (strcmp(token, "vn") == 0)
				{
					vector3 normal;
					if (sscanf(lineHeader+3, "%f %f %f\n", &normal.x, &normal.y, &normal.z) == 3)
						temp_normals.push_back(normal);					
				}

				// if reading face info
				else if (strcmp(token, "f") == 0)
				{
					std::vector<std::string>faceStrings;
					while (token != NULL)
					{
						// add all unique vertex/texture coordinates/normal mappings to collection
						// for the Vertex Buffer Offset
						if (strcmp(token, "f") != 0)
						{
							// if current vertex doesn't exist on vertex-index map
							if (vertexToIndex.find(token) == vertexToIndex.end())
							{
								// add it to the collection and increment the index
								vertexToIndex.emplace_hint(vertexToIndex.end(), token, vertexIndex);
								vertexIndex++;
								uniqueVertStrings.push_back(token);
							}

							faceStrings.push_back(token);
						}
						

						token = strtok(NULL, " \n");						
					}
					
					uint8_t isQuad = faceStrings.size() == 4;					
					uint32_t buf[3] = { 0, 0, 0 };					
					std::vector<uint32_t> triangulate_vertices, triangulate_uvs, triangulate_normals;
					// Parse faceIndices for vertex, uv, normal
					for (std::vector<std::string>::iterator it = faceStrings.begin(); it != faceStrings.end(); it++)
					{
						vertex tempVert;
						// if the face is a quad
						if (isQuad)
						{
							if (textureCoordsUsed)
							{
								if (sscanf(it->c_str(), "%d/%d/%d", &buf[0], &buf[1], &buf[2]) == 3)
								{
									triangulate_vertices.push_back(buf[0]);
									triangulate_uvs.push_back(buf[1]);
									triangulate_normals.push_back(buf[2]);
								}
							}

							else
							{
								if (sscanf(it->c_str(), "%d//%d", &buf[0], &buf[1]) == 2)
								{
									triangulate_vertices.push_back(buf[0]);
									triangulate_normals.push_back(buf[1]);
								}
							}

							// Triangulate the quad at the last iteration of the collection
							if ((it +1) == faceStrings.end())
							{
								int tempBuf[] = { 0, 1, 2, 2, 0, 3 };
								for (int i = 0; i < int(sizeof(tempBuf) / sizeof(int)); i++)
								{
									std::string face = "";
									int index = tempBuf[i];
									vertexIndices.push_back(triangulate_vertices[index]);
									face += std::to_string(triangulate_vertices[index]) + "/";
									if (textureCoordsUsed)
									{
										uvIndices.push_back(triangulate_uvs[index]);
										face += std::to_string(triangulate_uvs[index]);
									}
									face += "/";
									normalIndices.push_back(triangulate_normals[index]);
									face += std::to_string(triangulate_normals[index]);

									// add index of vertex to index buffer
									indices.push_back((uint16_t)std::distance(uniqueVertStrings.begin(), std::find(uniqueVertStrings.begin(), uniqueVertStrings.end(), face)));									
									
								}
							}
							
						}
						else
						{
							indices.push_back((uint16_t)std::distance(uniqueVertStrings.begin(), std::find(uniqueVertStrings.begin(), uniqueVertStrings.end(), *it)));
							if (textureCoordsUsed)
							{
								if (sscanf(it->c_str(), "%d/%d/%d", &buf[0], &buf[1], &buf[2]) == 3)
								{
									vertexIndices.push_back(buf[0]);
									uvIndices.push_back(buf[1]);
									normalIndices.push_back(buf[2]);
								}
							}

							else
							{
								if (sscanf(it->c_str(), "%d//%d", &buf[0], &buf[1]) == 2)
								{
									vertexIndices.push_back(buf[0] - 1);
									normalIndices.push_back(buf[1] - 1);
								}
							}							
						}			
						
					}
				}
			}
			// Clear the buffer for the line header
			memset(lineHeader, '\0', sizeof(lineHeader));
			lineIndex++;				
		}
		printf("\n");

		printf("\rLoading vertices... %d%%", int(0.0f / indices.size() * 100));		

		int i = 0;
		for (std::vector<std::string>::iterator it = uniqueVertStrings.begin(); it != uniqueVertStrings.end(); it++)
		{			
			int buf[3] = { 0 };
			vertex tempVert;
			if (textureCoordsUsed)
			{				
				if (sscanf(it->c_str(), "%d/%d/%d", &buf[0], &buf[1], &buf[2]) == 3)
				{
					tempVert = vertex(
						temp_vertices[buf[0] - 1],
						temp_uvs[buf[1] - 1],
						temp_normals[buf[2] - 1]);					
				}

			}
			else
			{
				if (sscanf(it->c_str(), "%d//%d", &buf[0], &buf[1]) == 2)
				{
					tempVert = vertex(
						temp_vertices[buf[0] - 1],
						temp_normals[buf[1] - 1]);					
				}

			}			
			printf("\rLoading vertices... %d%%", int((float)(i+1) / indices.size() * 100));
			
			vertices.push_back(tempVert);
			i++;
		}

		printf("\n");

		fclose(file);
		return true;		
	}

	perror("Unable to read obj file!");
	return false;
}
bool LoadObj(const char* filePath, std::vector<vertex>& vertices, std::vector<unsigned short>& indices)
{
	std::vector<vector3>temp_verts;
	std::vector<vector2>temp_uvs;
	std::vector<vector3>temp_normals;

	std::ifstream file(filePath);
	if (file)
	{
		unsigned short index = 0;
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string line;
		float x, y, z;
		while (std::getline(buffer, line))
		{
			// if reading positions
			if (strcmp(line.substr(0,2).c_str(), "v ") == 0)
				if (sscanf(line.c_str() + 2, "%f %f %f", &x, &y, &z) == 3)
					temp_verts.push_back(vector3(x, y, z));

			// if reading texCoords
			if (strcmp(line.substr(0, 2).c_str(), "vt") == 0)
				if (sscanf(line.c_str() + 3, "%f %f", &x, &y) == 2)
					temp_uvs.push_back(vector2(x, y));

			// if reading normals
			if (strcmp(line.substr(0, 2).c_str(), "vn") == 0)
				if (sscanf(line.c_str() + 3, "%f %f %f", &x, &y, &z) == 3)
					temp_normals.push_back(vector3(x, y, z));

			// if reading faces
			if (*line.c_str() == 'f')
			{				
				std::string dataLine = line.substr(2);
				// count the number of spaces in the line
				int vertsPerFace = (int)std::count(dataLine.begin(), dataLine.end(), ' ') + 1;
				std::stringstream tokenLine(dataLine);
				std::string token;
				while (getline(tokenLine, token, ' '))
				{
					int tempIndexBuf[3];
					vertex tempVertex;
					if (sscanf(token.c_str(), "%d/%d/%d", &tempIndexBuf[0], &tempIndexBuf[1], &tempIndexBuf[2]) == 3)
					{
						tempVertex = vertex(temp_verts[tempIndexBuf[0] - 1],
											temp_uvs[tempIndexBuf[1] - 1],
											temp_normals[tempIndexBuf[2] -1]);
						vertices.push_back(tempVertex);
					}
					else if (sscanf(token.c_str(), "%d//%d", &tempIndexBuf[0], &tempIndexBuf[1]) == 2)
					{
						tempVertex = vertex(temp_verts[tempIndexBuf[0] - 1],							
											temp_normals[tempIndexBuf[1] - 1]);

						vertices.push_back(tempVertex);
					}
				}

				// if the face is a quad
				if (vertsPerFace == 4)
				{
					// Triangulate the quad
					int triIndices[] = { 0, 1, 2, 2, 3, 0 };

					for (int i = 0; i < 6; i++)
					{
						// TEMPORARY SOLUTION
						// Will not work if there are duplicate vertices
						indices.push_back(index + triIndices[i]);
					}
					index += 4;
				}
				// otherwise store the data normal because it is a tri
				else
				{
					for (int i = 0; i < 3; i++)
					{
						indices.push_back(index);
						index++;
					}
				}

			}
		}

		return true;
	}
	else
	{
		perror("Unable to read file!");
		return false;
	}
}
// Calculates and returns the projection matrix from the field of view
// proved
mat4 GetProjectionMatrix(const float& _fov, const float& aspectRatio)
{
	return glm::perspective(glm::radians(_fov), aspectRatio, 0.1f, 100.0f);
}

// Calculates and returns the view matrix from the parameters provided
mat4 GetViewMatrix(const vector3& pos, const vector3& dir, const vector3 up)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return glm::lookAt(
				pos,       // camera position
				pos + dir, // where exactly the camera is looking
				up);       // where the camera's up vector is
}
