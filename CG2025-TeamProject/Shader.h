#pragma once
#include "ShaderUtils.h"
namespace Graphics {
	class Shader {
	public:
		Shader(std::string fragmentPath, std::string vertexPath);

		GLuint GetShaderProgramID() const { return shaderProgramID; }
		void Activate() { glUseProgram(shaderProgramID); }

		// 유틸 함수들
		void SetMat4(const char* name, const glm::mat4& mat) const {
			GLint loc = glGetUniformLocation(shaderProgramID, name);
			if (loc != -1)
				glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
		}

		void SetVec3(const char* name, const glm::vec3& vec) const {
			GLint loc = glGetUniformLocation(shaderProgramID, name);
			if (loc != -1)
				glUniform3fv(loc, 1, glm::value_ptr(vec));
		}

		void SetInt(const char* name, int value) const {
			GLint loc = glGetUniformLocation(shaderProgramID, name);
			if (loc != -1)
				glUniform1i(loc, value);
		}
	private:
		GLuint shaderProgramID;
	};
}