#include "glPCH.h"
#include "Color.h"
#include "MathLibrary.h"

float* MColor::ToFloatArray()
{
	static float colors[4];
    colors[0] = MathLibrary::Lerp(0.0f, 1.0f, (float)r / 255);
    colors[1] = MathLibrary::Lerp(0.0f, 1.0f, (float)g / 255);
    colors[2] = MathLibrary::Lerp(0.0f, 1.0f, (float)b / 255);
    colors[3] = MathLibrary::Lerp(0.0f, 1.0f, (float)a / 255);
    return colors;
}

float* MColor::ToFloatArray(const MColor& color)
{
	static float colors[4];
	colors[0] = MathLibrary::Lerp(0.0f, 1.0f, (float)color.r /255);
	colors[1] = MathLibrary::Lerp(0.0f, 1.0f, (float)color.g / 255);
	colors[2] = MathLibrary::Lerp(0.0f, 1.0f, (float)color.b / 255);
	colors[3] = MathLibrary::Lerp(0.0f, 1.0f, (float)color.a / 255);
	return colors;
}

MLinearColor MColor::ToLinearColor(const MColor& color)
{
	return MLinearColor(MathLibrary::Lerp(0.0f, 1.0f, (float)color.r / 255),
						MathLibrary::Lerp(0.0f, 1.0f, (float)color.g / 255),
						MathLibrary::Lerp(0.0f, 1.0f, (float)color.b / 255),
						MathLibrary::Lerp(0.0f, 1.0f, (float)color.a / 255));
}

float* MLinearColor::ToFloatArray()
{
	static float colors[4]{ r, g, b, a };
	return colors;
}

float* MLinearColor::ToFloatArray(const MLinearColor& color)
{
	static float colors[4]{ color.r, color.g, color.b, color.a };
	return colors;
}
