#pragma once
#include "Graphic.h"
#include "Shader.h"
#include "Color.h"
struct Line
{
    vector3 startPoint   = vector3(0.0f);
    vector3 endPoint     = vector3(1.0f);
    
};

struct LineGLInfo
{
    vector3 position;
    float u;
};

class LineRenderer :
    public Graphic
{
    // Public Functions
public:
    LineRenderer();
    ~LineRenderer();
    virtual void Draw(mat4 projection, mat4 view) override;
    /// <summary>
    /// Linear interpolate the line from the start
    /// color to the end
    /// </summary>
    /// <param name="start">The start color</param>
    /// <param name="end">The end color</param>
    void LerpColors(const MLinearColor& _start, const MLinearColor& _end);
    /// <summary>
    /// Adds a line point to the current collection of line points
    /// </summary>
    /// <param name="point">line point being added</param>
    void AddPoint(const vector3& point);
    /// <summary>
    /// Clears the current collection of points and adds the new point
    /// to the collection
    /// </summary>
    /// <param name="point">line point being added</param>
    void AddNewPoint(const vector3& point);
    /// <summary>
    /// Adds a collection of line points to the current
    ///  collection of line points
    /// </summary>
    /// <param name="points">line points being added</param>
    void AddPoints(const std::vector<vector3>& points);
    void AddPoints(const vector3* points, const int& linePointCount);
    /// <summary>
    /// Clears the current collection of points and adds the new points
    /// to the collection
    /// </summary>
    /// <param name="points"></param>
    void AddNewPoints(const std::vector<vector3>& points);
    void AddNewPoints(const vector3* points, const int& linePointCount);
    /// <summary>
    /// Changes the color of the line renderer
    /// </summary>
    /// <param name="color"> The new line color</param>
    void SetLineColor(const MLinearColor& color);
    /// <summary>
    /// Sets the width of the line
    /// </summary>
    /// <param name="width"></param>
    void SetLineWidth(const float& width);
    /// <summary>
    /// Sets overall brightness of the line
    /// being rendered
    /// </summary>
    /// <param name="value"></param>
    void SetBrightness(const float& value);
    virtual bool operator==(const Graphic& other);
    // TODO: Change this back to private once this
    // class can only be used through composition

    // Private functions
private:
    /// <summary>
    /// Updates the line renderer's uvs to be passed into the glsl shader
    /// </summary>
    void UpdateUVs();
    /// <summary>
    /// Updates the lines array with the current
    ///  collection of line points
    /// </summary>
    void UpdateLines();
    /// <summary>
    /// Updates the buffer data being passed into 
    /// GLSL to be drawn
    /// </summary>
    void UpdateLineBuffer();
    // Update the transform info (EntityComponent parent class only has access to it)
    void UpdateTransform(const mat4& _transform);

    std::vector<vector3> ToVectorArray();
    // Private members
private:
    MLinearColor lineColor = White;
    MLinearColor startColor, endColor;
    bool lerpColor = false;
    float brightness = 1.0f;
    std::vector<vector3> linePoints;
    std::vector<Line> lines;
    std::vector<LineGLInfo> linesBuffer;
	mat4 cachedTransform = mat4(1.0f);
    Shader* shader = nullptr;
    int pointCount = 0;
    GLuint vao, vbo;

    friend class LineRenderComponent;
};

