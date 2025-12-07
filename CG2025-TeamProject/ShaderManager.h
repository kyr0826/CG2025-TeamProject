#pragma once
#include <map>
#include <string>
#include "Shader.h"

class ShaderManager {
public:
	static ShaderManager& GetInstance() {
		static ShaderManager instance;
		return instance;
	}

	Graphics::Shader* GetShader(std::string name);
	void Initialize();
private:
	struct ShaderInfo {
		std::string name;
		std::string vertex_path;
		std::string fragment_path;
	};

	ShaderManager();
	~ShaderManager();

	std::map<std::string, Graphics::Shader*> shaders;
};