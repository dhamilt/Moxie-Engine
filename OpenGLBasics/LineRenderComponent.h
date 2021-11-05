#pragma once
#include "EntityComponent.h"
#include "LineRenderer.h"
class LineRenderComponent :
    public EntityComponent
{
    // Public functions
public:
    LineRenderComponent();
    virtual void Update(double deltaTime) override;
    void SetLineWidth(float width);
    /// <summary>
    /// Returns the number of points in the line renderer
    /// </summary>
    /// <param name="points">List of all the points</param>
    /// <returns>The number of points in the line</returns>
    int GetPoints(std::vector<vector3>& points);
    // Public variables
public:
    LineRenderer* renderer = nullptr;


};

