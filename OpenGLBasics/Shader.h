#pragma once
#include <fstream>
#include <sstream>
#include <GL\glew.h>
#include "3DTypeDefs.h"
#include "Color.h"
class Shader
{
public:
	// id of the shader program
	unsigned int id;

	// new default constructor that accepts the vertex and fragment shader files
	Shader(const char* vertexPath, const char* fragmentPath);
	// Activate the shader
	void Use();
	// uniform utility functions
	void SetBool(const char* attributeName, const bool& val);
	void SetFloat(const char* attributeName, const float& val);
	void SetInt(const char* attributeName, const int& val);
	void SetMat4(const char* attributeName, const mat4& val);
	void SetFloat4(const char* attributeName, const vector4& val);
	void SetFloat4(const char* attributeName, const MLinearColor& color);
	void SetFloat4(const char* attributeName, const float& x, const float& y, const float& z, const float& w);
	void SetFloat3(const char* attributeName, const vector3& val);
	void SetFloat3(const char* attributeName, const MLinearColor& color);
	void SetFloat3(const char* attributeName, const float& x, const float& y, const float& z);
	void SetFloat2(const char* attributeName, const vector2& val);
	void SetFloat2(const char* attributeName, const float& x, const float& y);
private:
	// Hide the default constructor
	Shader(){}
	
};

inline Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::ifstream vertexShaderFile(vertexPath, std::ifstream::in);
	std::ifstream fragmentShaderFile(fragmentPath, std::ifstream::in);
	std::stringstream sstream;
	if (vertexShaderFile.is_open() && fragmentShaderFile.is_open())
	{
		// Strings representing the vertex and fragment shaders
		std::string vertexShaderString;
		std::string fragmentShaderString;

		// if vertex file path is valid
		if (vertexShaderFile)
		{
			sstream << vertexShaderFile.rdbuf();
			vertexShaderString = sstream.str();
		}

		// if fragment file path is valid
		if (fragmentShaderFile)
		{
			sstream.str(std::string());
			sstream << fragmentShaderFile.rdbuf();
			fragmentShaderString = sstream.str();
		}

		// string to string literal conversation
		const char* vsCode = vertexShaderString.c_str();
		const char* fsCode = fragmentShaderString.c_str();

		// compile shaders
		unsigned int vertexShader, fragmentShader;
		int result;
		char infoLog[512];

		// Compile vertex shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vsCode, NULL);
		glCompileShader(vertexShader);
		// print compile errors
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			printf("Error compiling vertex shader! %s\n", infoLog);
		}

		// Compile fragment shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fsCode, NULL);
		glCompileShader(fragmentShader);
		// print compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			printf("Error compiling fragment shader! %s\n", infoLog);
		}

		// Create shader program
		id = glCreateProgram();

		// Attach shaders to program and link it
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);

		// print any linking errors
		glGetProgramiv(id, GL_LINK_STATUS, &result);
		if (!result)
		{
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			printf("Error! Unable to link subshaders to program! %s\n", infoLog);
		}

		// Delete shaders as they already exist on the program
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		fragmentShaderFile.close();
		vertexShaderFile.close();
	}
	
}

inline void Shader::Use()
{
	glUseProgram(id);
}

inline void Shader::SetBool(const char* attributeName, const bool& val)
{
	glUniform1i(glGetUniformLocation(id, attributeName), (int)val);
}

inline void Shader::SetFloat(const char* attributeName, const float& val)
{
	glUniform1f(glGetUniformLocation(id, attributeName), val);
}

inline void Shader::SetInt(const char* attributeName, const int& val)
{
	glUniform1i(glGetUniformLocation(id, attributeName), val);
}

inline void Shader::SetMat4(const char* attributeName, const mat4& val)
{
	glUniformMatrix4fv(glGetUniformLocation(id, attributeName), 1, GL_FALSE, glm::value_ptr(val));
}

inline void Shader::SetFloat4(const char* attributeName, const vector4& val)
{
	glUniform4f(glGetUniformLocation(id, attributeName), val.x, val.y, val.z, val.w);
}

inline void Shader::SetFloat4(const char* attributeName, const float& x, const float& y, const float& z, const float& w)
{
	glUniform4f(glGetUniformLocation(id, attributeName), x, y, z, w);
}

inline void Shader::SetFloat4(const char* attributeName, const MLinearColor& color)
{
	glUniform4f(glGetUniformLocation(id, attributeName), color.r, color.g, color.b, color.a);
}

inline void Shader::SetFloat3(const char* attributeName, const vector3& val)
{
	glUniform3f(glGetUniformLocation(id, attributeName), val.x, val.y, val.z);
}

inline void Shader::SetFloat3(const char* attributeName, const MLinearColor& color)
{
	glUniform3f(glGetUniformLocation(id, attributeName), color.r, color.g, color.b);
}

inline void Shader::SetFloat3(const char* attributeName, const float& x, const float& y, const float& z)
{
	glUniform3f(glGetUniformLocation(id, attributeName), x, y, z);
}

inline void Shader::SetFloat2(const char* attributeName, const vector2& val)
{
	glUniform2f(glGetUniformLocation(id, attributeName), val.x, val.y);
}

inline void Shader::SetFloat2(const char* attributeName, const float& x, const float& y)
{
	glUniform2f(glGetUniformLocation(id, attributeName), x, y);
}
