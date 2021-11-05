#pragma once
#include <vector>
namespace Moxie
{
	enum class FramebufferPresets 
	{
	DEFAULT_SCENE = 0x0,
	BLOOM,
    FIRSTPASS_GAUSSIAN_BLUR,
	SECONDPASS_GAUSSIAN_BLUR,
	HDR,
	HDR32,
	CUSTOM
	};

	enum class InternalFormats
	{
		RGBA8,
		RGBA16F,
		RGBA32F
	};
	enum class PixelFormats
	{
		RGBA,
		RGB,
		RG,
		R
	};
	enum class PixelDataOutputTypes
	{
		FLOAT,
		UNSIGNED_BYTE,
		BYTE,
		UNSIGNED_INT,
		INT,
		UNSIGNED_SHORT,
		SHORT
	};

	struct FramebufferParams
	{
		FramebufferParams() {};
		FramebufferParams(const FramebufferPresets& fbPreset)
			:framebufferPreset(fbPreset)
		{
			switch (fbPreset)
			{
			case FramebufferPresets::HDR:
				internalFormat = InternalFormats::RGBA16F;
				break;
			case FramebufferPresets::HDR32:
				internalFormat = InternalFormats::RGBA32F;
				break;
			case FramebufferPresets::FIRSTPASS_GAUSSIAN_BLUR:
				internalFormat = InternalFormats::RGBA16F;
				break;
			case FramebufferPresets::SECONDPASS_GAUSSIAN_BLUR:
				internalFormat = InternalFormats::RGBA16F;
				break;
			case FramebufferPresets::BLOOM:
				internalFormat = InternalFormats::RGBA16F;
				break;
			}
		}
		FramebufferParams(unsigned int fbID, const std::vector<unsigned int>& _colorAttachments, const PixelFormats& _pixelFormat,
			const PixelDataOutputTypes& _dataType, const InternalFormats& _internalFormat, const FramebufferPresets& _framebufferPreset)
			:
			bufferId(fbID)
			,colorAttachments(_colorAttachments)
			,pixelFormat(_pixelFormat)
			,dataType(_dataType)
			,internalFormat(_internalFormat)
			,framebufferPreset(_framebufferPreset)
			{};
		unsigned int bufferId = 0;
		std::vector<unsigned int> colorAttachments;
		PixelFormats pixelFormat= PixelFormats::RGBA;
		PixelDataOutputTypes dataType = PixelDataOutputTypes::UNSIGNED_BYTE;
		InternalFormats internalFormat = InternalFormats::RGBA8;
		FramebufferPresets framebufferPreset = FramebufferPresets::DEFAULT_SCENE;
	};
}