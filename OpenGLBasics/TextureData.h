#pragma once

struct TextureData
{
public:
	int width = 0, height = 0, channels = 0;
	void* data = nullptr;
};