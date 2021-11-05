#include "glPCH.h"
#include "LineRenderer.h"
#include "MathLibrary.h"

LineRenderer::LineRenderer()
{
	type = typeid(*this).name();
	glEnable(GL_LINE_SMOOTH);
	shader = new Shader("Line.vertex", "Line.fragment");
}

LineRenderer::~LineRenderer()
{
	glDisable(GL_LINE_SMOOTH);
}

void LineRenderer::Draw(mat4 projection, mat4 view)
{
	if (vbo != NULL && vao != NULL)
	{
		assert(shader);
		shader->Use();
		shader->SetMat4("projection", projection);
		shader->SetMat4("view", view);
		shader->SetMat4("model", cachedTransform);
		shader->SetFloat4("lineColor", lineColor);
		shader->SetBool("_lerp", lerpColor);
		shader->SetFloat("brightnessFactor", brightness);
		if (lerpColor)
		{
			shader->SetFloat4("startColor", startColor);
			shader->SetFloat4("endColor", endColor);
		}



		// tell the gpu to draw lines
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, pointCount);
		glBindVertexArray(0);
	}
}

void LineRenderer::LerpColors(const MLinearColor& _start, const MLinearColor& _end)
{
	lerpColor = true;
	startColor = _start;
	endColor = _end;
}

void LineRenderer::AddPoint(const vector3& point)
{
	linePoints.push_back(point);
	UpdateLines();
}

void LineRenderer::AddNewPoint(const vector3& point)
{
	linePoints.clear();
	linePoints.push_back(point);
	UpdateLines();
}

void LineRenderer::AddPoints(const std::vector<vector3>& points)
{
	std::vector<vector3>::iterator it = linePoints.end();
	linePoints.insert(it, points.begin(), points.end());
	UpdateLines();
}

void LineRenderer::AddPoints(const vector3* points, const int& linePointCount)
{
	int i = 0;
	while (i < linePointCount)
	{
		linePoints.push_back(points[i]);
		i++;
	}
	UpdateLines();
}

void LineRenderer::AddNewPoints(const std::vector<vector3>& points)
{
	linePoints.clear();
	std::vector<vector3>::iterator it = linePoints.begin();
	linePoints.insert(it, points.begin(), points.end());
	UpdateLines();
}

void LineRenderer::AddNewPoints(const vector3* points, const int& linePointCount)
{
	linePoints.clear();
	int i = 0;
	while (i < linePointCount)
	{
		linePoints.push_back(points[i]);
		i++;
	}
	UpdateLines();
}

void LineRenderer::SetLineColor(const MLinearColor& color)
{
	lineColor = color;
}

void LineRenderer::SetLineWidth(const float& width)
{
	glLineWidth(width);
}

void LineRenderer::SetBrightness(const float& value)
{
	brightness = value;
}

bool LineRenderer::operator==(const Graphic& other)
{
	const LineRenderer* lrCMP = dynamic_cast<const LineRenderer*>(&other);
	if (!lrCMP) return false;
	return memcmp(lrCMP, this, sizeof(LineRenderer)) == 0;
}

void LineRenderer::UpdateUVs()
{
	// reset the line gl buffer
	linesBuffer.clear();

	// loop through the line points collection
	for (int i = 0; i < linePoints.size(); i++)
	{
		// Create a new line gl info instance each iteration 
		LineGLInfo info = LineGLInfo();
		// that takes in the current line point and make an
		info.position = linePoints[i];
		// interpolated value from beginning to end of the line
		// beginning = 0; end = 1
		info.u = MathLibrary::Lerp(0.0f, 1.0f, (float)i / (float)(linePoints.size() - 1));
		linesBuffer.push_back(info);
	}
}

void LineRenderer::UpdateLines()
{
	// clear the current collection
	lines.clear();
	pointCount = 0;

	// if there are 2 or more line points
	if (linePoints.size() > 1)
	{
		for (std::vector<vector3>::iterator it = linePoints.begin(); it != linePoints.end(); it++)
		{
			// if iterator is past the first index of the collection
			if (it != linePoints.begin())
			{
				Line line = Line();
				// use the previous value as the starting point of the line
				line.startPoint = *(it - 1);
				// and use the current value as the ending point of the line
				line.endPoint = *it;
				// then add it to the collection
				lines.push_back(line);
				pointCount += 2;
			}
		}

		// then update line's uvs
		UpdateUVs();
		// finally update the opengl buffer info with the new collection data
		UpdateLineBuffer();
	}
}

void LineRenderer::UpdateLineBuffer()
{
	// generate a vertex array and vertex buffer objects if not done already
	if (vao == NULL)
		glGenVertexArrays(1, &vao);
	if (vbo == NULL)
		glGenBuffers(1, &vbo);

	// Convert all line points to array
	std::vector<vector3>_lines = ToVectorArray();
	// Bind the data below to the vertex array object
	glBindVertexArray(vao);
	// pass in the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LineGLInfo)* linesBuffer.size(), &linesBuffer[0], GL_DYNAMIC_DRAW);

	// Add vertex positions for the line
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,													// start index
		3,													// number of dimensions
		GL_FLOAT,											// vertex buffer type
		GL_FALSE,											// normalized?
		sizeof(LineGLInfo),									// stride
		(void*)offsetof(LineGLInfo, LineGLInfo::position)	// buffer offset
	);
	// Add vertex u(v)s for the line
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,											// start index
		1,											// number of dimensions
		GL_FLOAT,									// vertex buffer type
		GL_FALSE,									// normalized?
		sizeof(LineGLInfo),							// stride
		(void*)offsetof(LineGLInfo, LineGLInfo::u)	// buffer offset
	);

}

void LineRenderer::UpdateTransform(const mat4& _transform)
{
	cachedTransform = _transform;
}

std::vector<vector3> LineRenderer::ToVectorArray()
{
	std::vector<vector3> result;

	for (int i = 0; i < lines.size(); i++)
	{
		result.push_back(lines[i].startPoint);
		result.push_back(lines[i].endPoint);
	}
	return result;
}
