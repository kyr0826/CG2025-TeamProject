#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TextureManager.h"

namespace Core {
	bool TextureManager::LoadTexture(const std::string& name, const char* path) {
		// 이미 로드된 텍스처인지 확인
		if (textures.find(name) != textures.end()) {
			std::cout << "Texture already loaded: " << name << std::endl;
			return true;
		}

		GLuint textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		stbi_set_flip_vertically_on_load(true); // Y축 반전

		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1) format = GL_RED;
			else if (nrComponents == 3) format = GL_RGB;
			else if (nrComponents == 4) format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// 텍스처 파라미터 설정
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);

			// 맵에 저장 (이름 -> ID)
			textures[name] = textureID;
			std::cout << "Texture Loaded: " << name << " (" << path << ")" << std::endl;
			return true;
		}
		else {
			std::cout << "Texture Failed to Load: " << path << std::endl;
			stbi_image_free(data);
			return false;
		}
	}

	GLuint TextureManager::GetTexture(const std::string& name) {
		if (textures.find(name) != textures.end()) { return textures[name]; }

		std::cerr << "Texture not found: " << name << std::endl;
		return 0; // 0은 보통 null 텍스처 ID
	}

	void TextureManager::Initialize() {
		const std::vector<TextureInfo> init_textures = {
			{"Dish", "Dish_Texture.png"},
			{"Apple", "Apple_Texture.png"},
			{"UI_Back", "UI.png"}
		};

		for (const auto& info : init_textures) {
			std::string full_path = TEXTURE_ROOT + "/" + info.path;
			LoadTexture(info.name, full_path.c_str());
		}
	}

	void TextureManager::CleanUp() {
		for (auto const& iter : textures) { glDeleteTextures(1, &iter.second); }
		textures.clear();
	}
}