#pragma once
#include "LineRenderComponent.h"
#include "AudioSource.h"
#include "Color.h"
class AudioEQComponent :
    public LineRenderComponent
{
public:
    AudioEQComponent() = delete;
    AudioEQComponent(vector3 _start, vector3 _end);
    virtual void Update(double deltaTime) override;
    int AddSource(AudioSource* _source);
    int PlaySource(bool looping = false);
    AudioSource* GetSource();
    int IsPlaying();
    void ChangeSampleCount(int newSampleCount);
    void ChangeMaxHeightVariance(float newHeight);
    void ChangeStartPoint(vector3 _start);
    void ChangeEndPoint(vector3 _end);
    void AddInterpolatePoints(int numOfPointsToLerp);
    void AddLerpColors(Color startColor, Color endColor);
    


private:
    float padding = 1.0f;
    float height = 1.0f;
    int numOfSamples = 2;
    int numOfInterpolatedPointsPerSample = 0;
    AudioSource* source = nullptr;
    vector3 startPoint;
    vector3 endPoint;
    std::vector<vector3> linePoints;
    std::vector<vector3> wavePoints;
    std::vector<vector3> smoothWavePoints;
    std::vector<float> samples;
};

