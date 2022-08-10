#include "glPCH.h"
#include "AudioEQComponent.h"
#include "TransformationLibrary.h"


AudioEQComponent::AudioEQComponent(DVector3 _start, DVector3 _end)
{
	startPoint = _start;
	endPoint = _end;
	linePoints.push_back(startPoint);
	linePoints.push_back(endPoint);
}
void AudioEQComponent::Update(double deltaTime)
{
	LineRenderComponent::Update(deltaTime);

	if (source->IsValid() && source->isPlaying())
	{
		source->GetSamples(numOfSamples, samples);
		if (samples.size() == numOfSamples)
		{
			wavePoints = linePoints;
			// TODO: Reset the interpolated wave points instead clearing the queue
			smoothWavePoints.clear();
			for (int i = 0; i < numOfSamples; i++)
			{					
				wavePoints[i + 1].y +=  samples[i] * height;
				if (numOfInterpolatedPointsPerSample > 0)
				{
					// add the starting point for the interpolation
					DVector3 startingLerpPoint = wavePoints[i];
					DVector3 endingLerpPoint = wavePoints[i + 1];
					smoothWavePoints.push_back(startingLerpPoint);
					for (int j = 0; j < numOfInterpolatedPointsPerSample; j++)
					{
						// Created a interpolated point and add it to the collection
						DVector3 lerpedPoint = TransformationLibrary::Lerp(startingLerpPoint, endingLerpPoint, (float)(j + 1) / (numOfInterpolatedPointsPerSample + 1));
						smoothWavePoints.push_back(lerpedPoint);
					}
					// add the ending point for the interpolation
					smoothWavePoints.push_back(endingLerpPoint);
				}
			}

			// if interpolating points to smooth out
			// the waveform, 
			if (numOfInterpolatedPointsPerSample > 0)
			{
				//// add the ending point of the line to the collection
				//smoothWavePoints.push_back(endPoint);

				// add the collection to
				// the line renderer to draw out
				renderer->AddNewPoints(smoothWavePoints);
			}

			// if not interpolating points just add
			// the line points to the line renderer to draw
			else
				renderer->AddNewPoints(wavePoints);
		}
	}
}

int AudioEQComponent::AddSource(AudioSource* _source)
{
	source = _source;
	
	return source->IsValid();
}

int AudioEQComponent::PlaySource(bool looping)
{
	if (source)
		source->Play(looping);

	return source->IsValid();
}

AudioSource* AudioEQComponent::GetSource()
{
	if (source->IsValid())
		return source;
	return nullptr;
}

int AudioEQComponent::IsPlaying()
{
	return source->isPlaying();
}

void AudioEQComponent::ChangeSampleCount(int newSampleCount)
{
	numOfSamples = newSampleCount;
	if (numOfSamples != linePoints.size())
	{
		linePoints.clear();

		linePoints.push_back(startPoint);

		for (int i = 0; i < numOfSamples; i++)
		{						
			DVector3 temp = TransformationLibrary::Lerp(startPoint, endPoint, (float)(i+1) / (numOfSamples + 2));
			linePoints.push_back(temp);			
		}

		linePoints.push_back(endPoint);
	}
}

void AudioEQComponent::ChangeMaxHeightVariance(float newHeight)
{
	height = newHeight;
}

void AudioEQComponent::ChangeStartPoint(DVector3 _start)
{
	startPoint = _start;
	linePoints[0] = startPoint;
}

void AudioEQComponent::ChangeEndPoint(DVector3 _end)
{
	endPoint = _end;
	int size = numOfSamples - 1;
	linePoints[size] = endPoint;
}

void AudioEQComponent::AddInterpolatePoints(int numOfPointsToLerp)
{
	numOfInterpolatedPointsPerSample = numOfPointsToLerp;

	/*for (int i = 1; i < numOfSamples; i++)
	{
		std::vector<DVector3> temp;
		for (int j = 0; j < numOfInterpolatedPointsPerSample; j++)
			temp.push_back(TransformationLibrary::Lerp(linePoints[i-1], linePoints[i], (float)(j + 1) / (numOfInterpolatedPointsPerSample + 2)));

		linePoints.insert((linePoints.begin() + i), temp.begin(), temp.end());
	}*/
}

void AudioEQComponent::AddLerpColors(Color startColor, Color endColor)
{
	renderer->LerpColors(startColor, endColor);
}
