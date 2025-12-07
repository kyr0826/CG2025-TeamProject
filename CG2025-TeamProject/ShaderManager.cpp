#include "ShaderManager.h"

Graphics::Shader* ShaderManager::GetShader(std::string name) {
	return shaders[name];
}

void ShaderManager::Initialize() {
	ShaderInfo pendingShaders[] = {
		{"common_shader","vertex.glsl","fragment.glsl"},
		{"trajectory_shader","traj_vert.glsl","traj_frag.glsl"},
	};

	for (auto info : pendingShaders) {
		shaders[info.name] = new Graphics::Shader(info.fragment_path, info.vertex_path);
	}
}

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
	for (auto sh : shaders)
		delete sh.second;
}
