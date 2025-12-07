#pragma once
#include "Common_Headers.h"
#include "Shader.h"
#include "GameManager.h"
#include "TextureManager.h"

namespace Graphics {
	class UIManager {
	public:
		static UIManager& GetInstance() {
			static UIManager instance;
			return instance;
		}

		void Initialize(Shader* shader);
		void Render(int windowWidth, int windowHeight);

	private:
		UIManager();
		~UIManager();

		Shader* common_shader;
		GLuint quadVAO, quadVBO;
		GLuint bgTextureID;
		GLuint numTextures[10]; // 숫자 텍스처 ID 캐싱

		void ConfigureQuad();

		// 기본 그리기 함수 (전체 이미지)
		void RenderQuad(GLuint texID, float x, float y, float w, float h, glm::vec3 color = glm::vec3(1.0f));

		// 숫자 그리기
		void RenderNumber(int number, float x, float y, float scale);
	};
}