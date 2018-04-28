#pragma once
#include "GL\glew.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	GLuint Program;

	 GLuint load(const GLchar* shaderPath, GLenum type) {
		std::string shaderCode;
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// Open files
			shaderFile.open(shaderPath);
			std::stringstream shaderStream;
			// Read file's buffer contents into streams
			shaderStream << shaderFile.rdbuf();
			// close file handlers
			shaderFile.close();
			// Convert stream into string
			shaderCode = shaderStream.str();
		}
		catch (const std::exception&)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* cShaderCode = shaderCode.c_str();
		// 2. Compile shaders
		GLuint shader;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		shader = glCreateShader(type);
		glShaderSource(shader, 1, &cShaderCode, NULL);
		glCompileShader(shader);
		if (type == GL_VERTEX_SHADER)
		{
			checkCompileErrors(shader, "VERTEX");
		}
		else if (type == GL_FRAGMENT_SHADER)
		{
			checkCompileErrors(shader, "FRAGMENT");
		}
		else if (type == GL_GEOMETRY_SHADER)
		{
			checkCompileErrors(shader, "GEOMETRY");
		}
		else if (type == GL_TESS_CONTROL_SHADER)
		{
			checkCompileErrors(shader, "TESS_CONTROL");
		}
		else if (type == GL_TESS_EVALUATION_SHADER)
		{
			checkCompileErrors(shader, "TESS_EVALUATION");
		}

		return shader;
	}

	 static GLuint ComputeProgram(const GLchar* shaderPath) {
		 std::string shaderCode;
		 std::ifstream shaderFile;
		 shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		 try
		 {
			 // Open files
			 shaderFile.open(shaderPath);
			 std::stringstream shaderStream;
			 // Read file's buffer contents into streams
			 shaderStream << shaderFile.rdbuf();
			 // close file handlers
			 shaderFile.close();
			 // Convert stream into string
			 shaderCode = shaderStream.str();
		 }
		 catch (const std::exception&)
		 {
			 std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		 }
		 const GLchar* cShaderCode = shaderCode.c_str();
		 // 2. Compile shaders
		 GLuint shader;
		 GLint success;
		 GLchar infoLog[512];
		 // Vertex Shader
		 shader = glCreateShader(GL_COMPUTE_SHADER);
		 glShaderSource(shader, 1, &cShaderCode, NULL);
		 glCompileShader(shader);
		 glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		 if (!success)
		 {
			 glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			 std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << "COMPUTE_SHADER" << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		 }
		 GLuint program = glCreateProgram();
		 glAttachShader(program, shader);
		 glLinkProgram(program);
		 glGetProgramiv(shader, GL_LINK_STATUS, &success);
		 if (!success)
		 {
			 glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			 std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << "COMPUTE_SHADER" << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		 }
		 glDeleteShader(shader);
		 return program;
	 }

	// Constructor generates the shader on the fly
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr, const GLchar* tessllationControlPath = nullptr, const GLchar* tessllationEvaluationPath = nullptr)
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::string tcsCode;
		std::string tesCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		std::ifstream tcShaderFile;
		std::ifstream teShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			// If geometry shader path is present, also load a geometry shader
			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
			if (tessllationControlPath != nullptr)
			{
				tcShaderFile.open(tessllationControlPath);
				std::stringstream stream;
				stream << tcShaderFile.rdbuf();
				tcShaderFile.close();
				tcsCode = stream.str();
			}
			if (tessllationEvaluationPath != nullptr)
			{
				teShaderFile.open(tessllationEvaluationPath);
				std::stringstream stream;
				stream << teShaderFile.rdbuf();
				teShaderFile.close();
				tesCode = stream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar * fShaderCode = fragmentCode.c_str();
		// 2. Compile shaders
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// If geometry shader is given, compile geometry shader
		GLuint geometry;
		if (geometryPath != nullptr)
		{
			const GLchar * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		GLuint control;
		if (tessllationControlPath != nullptr)
		{
			const GLchar * cShaderCode = tcsCode.c_str();
			control = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(control, 1, &cShaderCode, NULL);
			glCompileShader(control);
			checkCompileErrors(control, "TESS_CONTROL");
		}
		GLuint evaluation;
		if (tessllationEvaluationPath != nullptr)
		{
			const GLchar * code = tesCode.c_str();
			evaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(evaluation, 1, &code, NULL);
			glCompileShader(evaluation);
			checkCompileErrors(evaluation, "TESS_EVALUATION");
		}
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		if (geometryPath != nullptr) {
			glAttachShader(this->Program, geometry);
		}
		if (tessllationControlPath != nullptr)
		{
			glAttachShader(this->Program, control);
		}
		if (tessllationEvaluationPath != nullptr)
		{
			glAttachShader(this->Program, evaluation);
		}
		glLinkProgram(this->Program);
		checkCompileErrors(this->Program, "PROGRAM");
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr) {
			glDeleteShader(geometry);
		}
		if (tessllationControlPath != nullptr)
		{
			glDeleteShader(control);
		}
		if (tessllationEvaluationPath != nullptr)
		{
			glDeleteShader(evaluation);
		}

	}
	Shader() {}
	// Uses the current shader
	void Use() { glUseProgram(this->Program); }
	void clean() { glDeleteProgram(Program); }
private:
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
			}
		}
	}
};