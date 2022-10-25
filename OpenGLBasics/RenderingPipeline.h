#pragma once
#include "glPCH.h"
#include <unordered_map>
#include "Shader.h"
#include "TextureData.h"
#include "Light.h"

class EntityComponent;
class Camera;
class GLSetup;
class Material;
class Mesh;
struct CubemapData;

struct RenderBufferData
{
	~RenderBufferData();
	bool hasTexCoords = false;
	Color color;
	GLuint vao, vbo, ebo;
	std::vector<DVertex> vertices;
	std::vector<uint16_t> indices;
	DMat4x4 transform;
	std::shared_ptr<Shader> shader;
};

// Rendering pipeline to carry out rendering tasks requested on multiple platforms
class BRenderingPipeline final
{
	BRenderingPipeline();
	~BRenderingPipeline();
	// Initialization function for setting up default framebuffer and renderbuffer
	void Init();
	/** Vertex specification Functions*/
	// Imports mesh data for pipeline to render 
	void Import(std::string primitiveName, std::vector<DVertex>_vertices, std::vector<uint16_t>_indices);
	void Import(std::string primitiveName, Mesh* mesh);
	void GenerateCubemap(std::vector<TextureData*>cubemapTextureData);
	// Feeds mesh data to rendering platform
	void RequestForMeshVertexData(std::string primitiveName);
	// Feeds vertex data from skybox to rendering platform
	void RequestForDefaultSkyboxVerts();
	void RequestForSkyboxVerts(std::string skyboxName, CubemapData* vertices);
	// Toggles projection from perspective to othrographic
	void SetOrthoView(bool isOrtho);
	// Updates the transform of the primitive
	void UpdateModelMatrix(DMat4x4 _transform, std::string primitiveName);
	// Updates the world space view matrix
	void UpdateProjectionMatrix(float fieldOfView, float width, float height, float nearClippingPlane, float farClippingPlane);
	void UpdateViewMatrix(DMat4x4 _view);
	/** Vertex Shading Functions */
	// Create default mesh shader
	void CreateDefaultShader();
	// Adds or updates shader to be used by primitive
	void LoadShader(std::string primitiveName, Material* mat);
	// Getter function for default shader
	void GetDefaultShader(Shader* defaultShader);
	// Adds shader to cubemap
	void LoadDefaultCubemapShader();
	// Adds shader to the cache
	void UpdateShaderCache(Shader* shaderToAdd);
	void UpdateShaderCache(Material* shaderToAdd);
	void UpdateDefaultCubemapShader();
	// Request to add a light to the scene
	bool RequestForLight(Light& light);
	// Passes in light data for shader
	void UpdateLightDataForShader(RenderBufferData* renderData);
	/** Vertex Post Processing Functions */

	/** Drawing Functions */
	// Generates framebuffer
	void GenerateDefaultFramebuffer();
	// Loads in framebuffer
	// set to 0 for default framebuffer
	void LoadFramebuffer(GLuint fbID);
	// Loads current framebuffer
	void LoadCurrentFramebuffer();
	//Unloads framebuffer
	void UnloadFramebuffer();
	// Draws all primitives to the screen for one frame
	void RenderPrimitives();
	// Draw function for rendering meshes
	void DrawMesh(RenderBufferData* renderData);
	// Draw function for rendering cubemaps
	void DrawCubeMap();

public:
	static const int MAX_LIGHT_COUNT;
private:
	// TODO: Create a class/struct that instructs the pipeline 
	// what the intended usage of the framebuffer is
	GLuint defaultFramebuffer, currentFramebuffer, renderbuffer;
	CubemapData* cubemapParams;
	Shader* cubemapShader;
	GLuint defaultFrameBufferTextureID;
	int screenWidth, screenHeight;
	std::vector<GLuint> framebuffers;
	std::unordered_map<std::string, RenderBufferData*>primitives;
	std::vector<Light*> lightCache;
	DMat4x4 projectionMatrix, viewMatrix;
	bool perspectiveView = true;
	std::vector<std::shared_ptr<Shader>> shaderCache;
	friend class EntityComponent;
	friend class Camera;
	friend class GLSetup;
};

