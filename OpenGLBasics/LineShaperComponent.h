#pragma once
#include "LineRenderComponent.h"
enum class Shapes {Sine, Cosine, Circle, Square, Sphere, MaurerRose};
class LineShaperComponent :
    public LineRenderComponent
{
public:
    LineShaperComponent();
    virtual void Update(double deltaTime)override;
    /// <summary>
    /// Sets the scale factor of the shape 
    /// </summary>
    /// <param name="value"></param>
    void SetScale(const float& value);
    
    /// <summary>
    /// Sets the number of revolutions in the line
    /// </summary>
    /// <param name="value"></param>
    void SetRevolutionCount(const float& value);
    /// <summary>
    /// Sets the scale of the time
    /// </summary>
    /// <param name="value"></param>
    void SetTimeScale(const float& value);
    /// <summary>
    /// Offsets the time passed in the accumulated 
    /// delta
    /// </summary>
    /// <param name="value">the time offset</param>
    void SetTimeOffset(const float& value);
    /// <summary>
    /// Set the shape the line will form
    /// </summary>
    /// <param name="shape">Shape of the line</param>
    void SetShape(const Shapes& _shape);

private:
    float scale = 1.0f;
    float numOfRevolutions = 1.0f;
    std::vector<DVector3>points;
    Shapes shape;
    double accumulatedDelta = 0.0f;
    float timeOffset;
    float maurerTimeDelta = 0.0f;
    bool increasePetalCount = true;
    int currentPetalCount = 1;
    int maxPetalCount = 12;
    int minPetalCount = 1;
    float timeScale = 1.0f;
};

