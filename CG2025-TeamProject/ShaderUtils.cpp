#define _CRT_SECURE_NO_WARNINGS
#include "ShaderUtils.h"

namespace Graphics {
	char* ShaderUtils::filetobuf(const char* file) {
		FILE* fptr;
		long length;
		char* buf;

		fptr = fopen(file, "rb");			//--- 바이너리 모드로 파일 열기
		if (!fptr)							//--- 파일 열기 실패
			return NULL;

		fseek(fptr, 0, SEEK_END);			//--- 파일 포인터를 파일 끝으로 이동
		length = ftell(fptr);				//--- 파일 길이 얻기
		buf = (char*)malloc(length + 1);	//--- 파일 길이 + NULL 문자
		fseek(fptr, 0, SEEK_SET);			//--- 파일 포인터를 파일 처음으로 이동
		fread(buf, length, 1, fptr);		//--- 파일 읽기
		fclose(fptr);						//--- 파일 닫기
		buf[length] = 0;					//--- NULL 문자 추가

		return buf;
	}

	bool ShaderUtils::make_vertexShaders(const char* file, GLuint& vertexShader) {
		GLchar* vertexSource = filetobuf(file);

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);

		GLint result;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

		if (!result) {
			char errorLog[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
			std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
			return false;
		}
		free(vertexSource);
		return true;
	}

	bool ShaderUtils::make_fragmentShaders(const char* file, GLuint& fragmentShader)
	{
		GLchar* fragmentSource = filetobuf("fragment.glsl");

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

		glCompileShader(fragmentShader);

		GLint result;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

		if (!result) {
			char errorLog[512];
			glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
			std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
			return false;
		}
		free(fragmentSource);
		return true;
	}
}