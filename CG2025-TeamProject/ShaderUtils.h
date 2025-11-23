#pragma once
#include "Common_Headers.h"

namespace Graphics {
	class ShaderUtils {
	public:
		static char* filetobuf(const char* file);
		static bool make_vertexShaders(const char* file, GLuint& vertexShader);
		static bool make_fragmentShaders(const char* file, GLuint& fragmentShader);
	};
}