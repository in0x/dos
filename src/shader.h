#pragma once
#include "dos.h"

std::vector<hmm_vec3> vertices = {
	// bottom
	{ 0.5f, -0.5f, 0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, -0.5f },

	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, -0.5f },

	// top 
	{ 0.5f, 0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ -0.5f, 0.5f, 0.5f },

	{ -0.5f, 0.5f, -0.5f },
	{ -0.5f, 0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f },

	// front
	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, 0.5f },

	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, 0.5f },
	{ -0.5f, 0.5f, 0.5f },

	// left
	{ -0.5f, -0.5f,  0.5f },
	{ -0.5f, 0.5f,  0.5f },
	{ -0.5f, -0.5f, -0.5f },

	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, 0.5f,  0.5f },
	{ -0.5f, 0.5f, -0.5f },

	// right
	{ 0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, -0.5f },
	{ 0.5f, 0.5f, -0.5f },

	{ 0.5f, -0.5f,  0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ 0.5f, 0.5f,  0.5f },
	// back
	{ 0.5f, -0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },

	{ 0.5f, -0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },
	{ 0.5f, 0.5f, -0.5f }
};

std::vector<hmm_vec3> normals = {
	{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },{ 0,-1, 0 },
	{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },{ 0, 1, 0 },
	{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },
	{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },{ -1, 0, 0 },
	{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },{ 1, 0, 0 },
	{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 },{ 0, 0, -1 }
};

void printShaderLog(GLuint shader)
{
	if (glIsShader(shader))
	{
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];

		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);

		if (infoLogLength > 0)
		{
			std::cout << infoLog << '\n';
		}

		delete[] infoLog;
	}

	else
	{
		std::cout << shader << " is not a shader" << '\n';
	}
}

GLuint createShader(const std::string& path, GLenum shaderType)
{
	std::string shaderString;
	std::ifstream fileStream(path);
	GLuint shader;

	if (fileStream)
	{
		shaderString.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());

		shaderString.erase(std::remove_if(shaderString.begin(), shaderString.end(),
			[](const char c) {
			return !(c >= 0 && c < 128);
		}
		), shaderString.end());

		const char* shaderSource = shaderString.c_str();

		shader = glCreateShader(shaderType);

		glShaderSource(shader, 1, (const char**)&shaderSource, NULL);

		glCompileShader(shader);
	}

	else
	{
		std::cout << "Failed to load shader" << '\n';
	}

	printShaderLog(shader);

	return shader;
}

