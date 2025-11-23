#pragma once
#include "ShaderUtils.h"
namespace Graphics {
	class Shader {
	public:
		Shader(std::string fragmentPath, std::string vertexPath);

		GLuint GetShaderProgramID() const { return shaderProgramID; }
		void Activate() { glUseProgram(shaderProgramID); }
	private:
		GLuint shaderProgramID;
	};
}