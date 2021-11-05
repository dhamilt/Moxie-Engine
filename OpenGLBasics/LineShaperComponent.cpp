#include "glPCH.h"
#include "LineShaperComponent.h"
#include "ShapeTransformationLibrary.h"

LineShaperComponent::LineShaperComponent()
{
	shape = Shapes::Sine;
}

void LineShaperComponent::Update(double deltaTime)
{
	LineRenderComponent::Update(deltaTime);

	int count = GetPoints(points);
	for (int i = 0; i < count; i++)
	{
		// if drawing a sinusoidal wave with lines
		if (shape == Shapes::Sine)
		{
			float sinusoidal = float(glm::sin((float)i * numOfRevolutions / count + (accumulatedDelta + timeOffset) * timeScale) * scale);			
			points[i].y = sinusoidal;
		}

		// if drawing a cosinusoidal wave with lines
		if (shape == Shapes::Cosine)
		{
			float cosinusoidal = float(glm::cos((float)i * numOfRevolutions / count + (accumulatedDelta + timeOffset) * timeScale) * scale);			
			points[i].y = cosinusoidal;
		}

		
	}

	if (shape == Shapes::MaurerRose)
	{		
		if (maurerTimeDelta > 360.0f)
		{
			maurerTimeDelta = float(deltaTime) * timeScale;
			if (increasePetalCount)
			{
				if (currentPetalCount >= maxPetalCount)			
					increasePetalCount = false;
				else
					currentPetalCount++;
			}

			if (!increasePetalCount)
			{
				if (currentPetalCount <= minPetalCount)
					increasePetalCount = true;
				else
					currentPetalCount--;
			}
		}
		else
			maurerTimeDelta += float(deltaTime) *timeScale;

		printf("\r%f", maurerTimeDelta);
		ShapeTransformationLibrary::GetMaurerRosePoints(currentPetalCount, maurerTimeDelta , .01f, .01f, vector3(0), points);
	}

	renderer->AddNewPoints(points);

	accumulatedDelta += deltaTime;
}

void LineShaperComponent::SetScale(const float& value)
{
	scale = value;
}

void LineShaperComponent::SetRevolutionCount(const float& value)
{
	numOfRevolutions = value;
}

void LineShaperComponent::SetTimeScale(const float& value)
{
	timeScale = value;
}

void LineShaperComponent::SetTimeOffset(const float& value)
{
	timeOffset = value;
}

void LineShaperComponent::SetShape(const Shapes& _shape)
{
	shape = _shape;
}
