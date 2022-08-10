#pragma once
#include "LineRenderComponent.h"
#include "AudioSource.h"
#include "Color.h"
class AudioEQComponent :
    public LineRenderComponent
{
public:
    AudioEQComponent() = delete;
    AudioEQComponent(DVector3 _start, DVector3 _end);
    virtual void Update(double deltaTime) override;
    int AddSource(AudioSource* _source);
    int PlaySource(bool looping = false);
    AudioSource* GetSource();
    int IsPlaying();
    void ChangeSampleCount(int newSampleCount);
    void ChangeMaxHeightVariance(float newHeight);
    void ChangeStartPoint(DVector3 _start);
    void ChangeEndPoint(DVector3 _end);
    void AddInterpolatePoints(int numOfPointsToLerp);
    void AddLerpColors(Color startColor, Color endColor);
    


private:
    float padding = 1.0f;
    float height = 1.0f;
    int numOfSamples = 2;
    int numOfInterpolatedPointsPerSample = 0;
    AudioSource* source = nullptr;
    DVector3 startPoint;
    DVector3 endPoint;
    std::vector<DVector3> linePoints;
    std::vector<DVector3> wavePoints;
    std::vector<DVector3> smoothWavePoints;
    std::vector<float> samples;
};

