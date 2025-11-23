#include "Shader.h"

namespace Graphics {
	Shader::Shader(std::string fragmentPath, std::string vertexPath) {
		GLuint fragment, vertex;
		ShaderUtils::make_fragmentShaders(fragmentPath.c_str(), fragment);
		ShaderUtils::make_vertexShaders(vertexPath.c_str(), vertex);

		shaderProgramID = glCreateProgram();
		glAttachShader(shaderProgramID, vertex);
		glAttachShader(shaderProgramID, fragment);
		glLinkProgram(shaderProgramID);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
}