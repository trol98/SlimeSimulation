#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "vendor/glm/glm/glm.hpp"


class Shader
{
	unsigned int m_ID;

public:
	Shader(const char* vertexPath, const char* fragmentPath) 
	{
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vertexShaderFile;
		std::ifstream fragmentShaderFile;
		
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vertexShaderFile.open(vertexPath);
			fragmentShaderFile.open(fragmentPath);

			std::stringstream vertexShaderStream, fragmentShaderStream;
			vertexShaderStream << vertexShaderFile.rdbuf();
			fragmentShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexCode = vertexShaderStream.str();
			fragmentCode = fragmentShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::puts("ERROR::SHADER::FAIL_NOT_SUCCESFULLY_READ");
		}

		const char* vertexShaderCode = vertexCode.c_str();
		const char* fragmentShaderCode = fragmentCode.c_str();

		unsigned vertexID, fragmentID;

		vertexID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexID, 1, &vertexShaderCode, nullptr);
		glCompileShader(vertexID);
		checkCompileErrors(vertexID, "VERTEX");

		fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentID, 1, &fragmentShaderCode, nullptr);
		glCompileShader(fragmentID);
		checkCompileErrors(fragmentID, "FRAGMENT");

		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexID);
		glAttachShader(m_ID, fragmentID);
		glLinkProgram(m_ID);
		checkCompileErrors(m_ID, "PROGRAM");

		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);
	}

	Shader(const char* computePath)
	{
		// this is a rather temporary solution to the problem of compiling compute shaders
		// in the future i'm going to change this becouse i don't like how this code is structured
		// it's not scalable, becouse if i ever will use teselation and teselation control shader or even geometry shader
		// this solution will become unmaintainable
		std::string computeCode;
		std::ifstream computeShaderFile;

		computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			computeShaderFile.open(computePath);

			std::stringstream computeShaderStream;
			computeShaderStream << computeShaderFile.rdbuf();

			computeShaderFile.close();

			computeCode = computeShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::puts("ERROR::SHADER::FAIL_NOT_SUCCESFULLY_READ");
		}

		const char* computerShaderCode = computeCode.c_str();

		unsigned computeID;

		computeID = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeID, 1, &computerShaderCode, nullptr);
		glCompileShader(computeID);
		checkCompileErrors(computeID, "COMPUTE");

		m_ID = glCreateProgram();
		glAttachShader(m_ID, computeID);
		glLinkProgram(m_ID);
		checkCompileErrors(m_ID, "PROGRAM");

		glDeleteShader(computeID);
	}


	void use() const
	{
		glUseProgram(m_ID);
	}
private:

	void checkCompileErrors(unsigned ID, const std::string& type) const
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(ID, 1024, nullptr, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION::ERROR of type: " << type << '\n' <<
					infoLog << '\n' << std::string(50, '-') << '\n';
			}
		}
		else
		{
			glGetProgramiv(ID, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << '\n' <<
					infoLog << '\n' << std::string(50, '-') << '\n';
			}
		}
	}
	int getUniformLocation(const std::string& name) const
	{
		int location = glGetUniformLocation(m_ID, name.c_str());
		// assert(location != -1)
		return location;
	}
public:
	void setMat4(const std::string& name, const glm::mat4& matrix) const
	{
		glUniformMatrix4fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &matrix[0][0]);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(getUniformLocation(name.c_str()), value);
	}
	void setVec3(const std::string& name, const glm::vec3& vector) const
	{
		glUniform3fv(getUniformLocation(name.c_str()), 1, &vector[0]);
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(getUniformLocation(name.c_str()), value);
	}

};