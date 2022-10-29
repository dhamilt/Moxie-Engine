#pragma once
#include <fstream>
#include <sstream>
#include <GL\glew.h>
#include "3DTypeDefs.h"
class Shader
{
public:
	// Hide the default constructor
	Shader() {}
	// new default constructor that accepts the vertex and fragment shader files
	Shader(const char* vertexPath, const char* fragmentPath);
	// Activate the shader
	void Use();
	// uniform utility functions
	void SetBool(const char* attributeName, const bool& val);
	void SetFloat(const char* attributeName, const float& val);
	void SetInt(const char* attributeName, const int& val);
	void SetMat3(const char* attributeName, const DMat3x3& val);
	void SetMat4(const char* attributeName, const DMat4x4& val);
	void SetFloat4(const char* attributeName, const DVector4& val);
	void SetFloat4(const char* attributeName, const float& x, const float& y, const float& z, const float& w);
	void SetFloat3(const char* attributeName, const DVector3& val);
	void SetFloat3(const char* attributeName, const float& x, const float& y, const float& z);
	void SetFloat2(const char* attributeName, const DVector2& val);
	void SetFloat2(const char* attributeName, const float& x, const float& y);
	int GetShaderID();
	bool operator==(Shader other) { return GetShaderID() == other.GetShaderID(); }
private:
	
	std::string vsPath;
	std::string fsPath;
	// id of the shader program
	unsigned int id;
	// shader parameter collections
	std::map<std::string, DMat3x3>	matrix3x3ParamsCollection;
	std::map<std::string, DMat4x4>	matrix4x4ParamsCollection;
	std::map<std::string, bool>		booleanParamsCollection;
	std::map<std::string, float>	floatParamsCollection;
	std::map<std::string, int>		integerParamsCollection;
	std::map<std::string, DVector4>	float4ParamsCollection;
	std::map<std::string, DVector3>	float3ParamsCollection;
	std::map<std::string, DVector2> float2ParamsCollection;
	
};

inline Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	vsPath = vertexPath;
	fsPath = fragmentPath;

	std::ifstream vertexShaderFile(vertexPath);
	std::ifstream fragmentShaderFile(fragmentPath);
	std::stringstream sstream;

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
}

inline void Shader::Use()
{
	glUseProgram(id);	
	
	// load all matrix4x4 data
	for (auto it = matrix4x4ParamsCollection.begin(); it != matrix4x4ParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		DMat4x4 val = it->second;
		glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(val));
	}

	// load all matrix3x3 data
	for (auto it = matrix3x3ParamsCollection.begin(); it != matrix3x3ParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		DMat3x3 val = it->second;
		glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(val));
	}

	// load all boolean data
	for (auto it = booleanParamsCollection.begin(); it != booleanParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		bool val = it->second;
		glUniform1i(glGetUniformLocation(id, name), (int)val);
	}
	
	// load all integer data
	for (auto it = integerParamsCollection.begin(); it != integerParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		int val = it->second;
		glUniform1i(glGetUniformLocation(id, name), val);
	}

	// load all float data
	for (auto it = floatParamsCollection.begin(); it != floatParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		float val = it->second;
		glUniform1f(glGetUniformLocation(id, name), val);
	}

	// load all float4 data
	for (auto it = float4ParamsCollection.begin(); it != float4ParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		DVector4 val = it->second;
		glUniform4f(glGetUniformLocation(id, name), val.x, val.y, val.z, val.w);
	}

	// load all float3 data
	for (auto it = float3ParamsCollection.begin(); it != float3ParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		DVector3 val = it->second;
		glUniform3f(glGetUniformLocation(id, name), val.x, val.y, val.z);
	}

	// load all float2 data
	for (auto it = float2ParamsCollection.begin(); it != float2ParamsCollection.end(); it++)
	{
		const char* name = it->first.c_str();
		DVector2 val = it->second;
		glUniform2f(glGetUniformLocation(id, name), val.x, val.y);
	}
}

inline void Shader::SetBool(const char* attributeName, const bool& val)
{
	booleanParamsCollection.insert_or_assign(attributeName, val);
}

inline void Shader::SetFloat(const char* attributeName, const float& val)
{
	floatParamsCollection.insert_or_assign( attributeName, val );
}

inline void Shader::SetInt(const char* attributeName, const int& val)
{
	integerParamsCollection.insert_or_assign( attributeName, val );
}

inline void Shader::SetMat3(const char* attributeName, const DMat3x3& val)
{
	matrix3x3ParamsCollection.insert_or_assign(attributeName, val);
}

inline void Shader::SetMat4(const char* attributeName, const DMat4x4& val)
{
	matrix4x4ParamsCollection.insert_or_assign( attributeName, val );
}

inline void Shader::SetFloat4(const char* attributeName, const DVector4& val)
{
	float4ParamsCollection.insert_or_assign( attributeName, val );
}

inline void Shader::SetFloat4(const char* attributeName, const float& x, const float& y, const float& z, const float& w)
{
	float4ParamsCollection.insert_or_assign( attributeName, DVector4(x, y, z, w) );
}

inline void Shader::SetFloat3(const char* attributeName, const DVector3& val)
{
	float3ParamsCollection.insert_or_assign( attributeName, val );
}

inline void Shader::SetFloat3(const char* attributeName, const float& x, const float& y, const float& z)
{
	float3ParamsCollection.insert_or_assign( attributeName, DVector3(x, y, z) );
}

inline void Shader::SetFloat2(const char* attributeName, const DVector2& val)
{
	float2ParamsCollection.insert_or_assign( attributeName, val );
}

inline void Shader::SetFloat2(const char* attributeName, const float& x, const float& y)
{
	float2ParamsCollection.insert_or_assign( attributeName, DVector2(x, y) );
}

inline int Shader::GetShaderID()
{
	return id;
}
