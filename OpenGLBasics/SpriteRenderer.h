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
		color = DVector4(1);
		size = DVector2(1);
		position = DVector2(0);
		rotation = 0.0f;
	}

	Sprite(Texture _texture, DVector4 _color, DVector2 _size, DVector2 _pos, float _rot) :color(_color), size(_size), position(_pos), rotation(_rot)
	{
		texture = &_texture;
	}
	Texture* texture;
	DVector4 color;
	DVector2 size;
	DVector2 position;
	float rotation;
};

class SpriteRenderer
{
public:
	SpriteRenderer() = delete;
	SpriteRenderer(Shader* _shader);
	~SpriteRenderer();
	void DrawSprite(Texture* texture, const DVector2 pos, const DVector2 size, const float rotate, const DVector4 color);
	void DrawSprite(const Sprite* sprite, const DMat4x4& projection, const DMat4x4& view);
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

inline void SpriteRenderer::DrawSprite(Texture* texture, const DVector2 pos, const DVector2 size, const float rotate, const DVector4 color)
{
	// prep transformations
	this->shader->Use();
	DMat4x4 model = DMat4x4(1.0f);
	model = glm::translate(model, DVector3(pos, 0.0f));

	model = glm::translate(model, DVector3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), DVector3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, DVector3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, DVector3(size, 1.0f));

	this->shader->SetMat4("model", model);
	this->shader->SetFloat4("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture->Bind();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

inline void SpriteRenderer::DrawSprite(const Sprite* sprite, const DMat4x4& projection, const DMat4x4& view)
{
	// prep transformations
	this->shader->Use();
	DMat4x4 model = DMat4x4(1.0f);
	model = glm::translate(model, DVector3(sprite->position, 0.0f));

	model = glm::translate(model, DVector3(0.5f * sprite->size.x, 0.5f * sprite->size.y, 0.0f));
	model = glm::rotate(model, glm::radians(sprite->rotation), DVector3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, DVector3(-0.5f *sprite->size.x, -0.5f * sprite->size.y, 0.0f));

	model = glm::scale(model, DVector3(sprite->size, 1.0f));

	//DMat4x4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

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
