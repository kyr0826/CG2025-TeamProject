#pragma once

#include "Common_Headers.h"
#include <map>

namespace Core {
	struct TextureInfo {
		std::string name;
		std::string path;
	};

	class TextureManager {
	public:
		static TextureManager& GetInstance() {
			static TextureManager instance;
			return instance;
		}
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;
		GLuint GetTexture(const std::string& name);
		//void Initialize();

		bool LoadTexture(const std::string& name, const char* path);
	private:
		std::map<std::string, GLuint> textures;
		TextureManager() {}
		~TextureManager() { CleanUp(); }
		
		void CleanUp();
	};
}