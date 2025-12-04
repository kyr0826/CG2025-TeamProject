#include "Shader.h"

namespace Graphics {
	Shader::Shader(std::string fragmentPath, std::string vertexPath) {
		GLuint fragment, vertex;
		if (!ShaderUtils::make_fragmentShaders(fragmentPath.c_str(), fragment)) {
			std::cerr << "Failed to compile fragment shader: " << fragmentPath << std::endl;
		}
		if (!ShaderUtils::make_vertexShaders(vertexPath.c_str(), vertex)) {
			std::cerr << "Failed to compile vertex shader: " << vertexPath << std::endl;
		}
		

		shaderProgramID = glCreateProgram();
		glAttachShader(shaderProgramID, vertex);
		glAttachShader(shaderProgramID, fragment);
		glLinkProgram(shaderProgramID);

		GLint success = 0;
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
		if (!success) {
			GLchar infoLog[512];
			glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
}