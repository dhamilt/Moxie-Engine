#pragma once
#include "glPCH.h"
#include "Texture.h"
#include "Shader.h"
struct Sprite
{
	Sprite()
	{
		TextureData* tData = new TextureData();
		ImageLibrary::GetDataFromFile("CircleMask.png", &tData);
		texture = new Texture(tData);		
		color = vector4(1);
		size = vector2(1);
		position = vector2(0);
		rotation = 0.0f;
	}

	Sprite(Texture _texture, vector4 _color, vector2 _size, vector2 _pos, float _rot) :color(_color), size(_size), position(_pos), rotation(_rot)
	{
		texture = &_texture;
	}
	Texture* texture;
	vector4 color;
	vector2 size;
	vector2 position;
	float rotation;
};

class SpriteRenderer
{
public:
	SpriteRenderer() = delete;
	SpriteRenderer(Shader* _shader);
	~SpriteRenderer();
	void DrawSprite(Texture* texture, const vector2 pos, const vector2 size, const float rotate, const vector4 color);
	void DrawSprite(const Sprite* sprite, const mat4& projection, const mat4& view);
private:
	void InitializeData();

private:
	Shader* shader;
	unsigned int quadVAO;

};

inline SpriteRenderer::SpriteRenderer(Shader* _shader)
{
	shader = _shader;
	InitializeData();
}

inline SpriteRenderer::~SpriteRenderer()
{
}

inline void SpriteRenderer::DrawSprite(Texture* texture, const vector2 pos, const vector2 size, const float rotate, const vector4 color)
{
	// prep transformations
	this->shader->Use();
	mat4 model = mat4(1.0f);
	model = glm::translate(model, vector3(pos, 0.0f));

	model = glm::translate(model, vector3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), vector3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, vector3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, vector3(size, 1.0f));

	this->shader->SetMat4("model", model);
	this->shader->SetFloat4("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture->Bind();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

inline void SpriteRenderer::DrawSprite(const Sprite* sprite, const mat4& projection, const mat4& view)
{
	// prep transformations
	this->shader->Use();
	mat4 model = mat4(1.0f);
	model = glm::translate(model, vector3(sprite->position, 0.0f));

	model = glm::translate(model, vector3(0.5f * sprite->size.x, 0.5f * sprite->size.y, 0.0f));
	model = glm::rotate(model, glm::radians(sprite->rotation), vector3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, vector3(-0.5f *sprite->size.x, -0.5f * sprite->size.y, 0.0f));

	model = glm::scale(model, vector3(sprite->size, 1.0f));

	//mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

	/*glBindTexture(GL_TEXTURE_2D, sprite->texture->textureID);*/

	this->shader->SetMat4("model", model);
	this->shader->SetMat4("projection", projection);
	this->shader->SetMat4("view", view);
	this->shader->SetFloat4("spriteColor", sprite->color);
		
	//sprite->texture->Bind();

	glBindVertexArray(this->quadVAO);
	sprite->texture->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

inline void SpriteRenderer::InitializeData()
{
	// configure VAO/VBO
	unsigned int VBO;

	float vertices[] = {
		// pos      // texCoords
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
