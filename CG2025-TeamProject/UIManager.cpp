#include "UIManager.h"

namespace Graphics {
	UIManager::UIManager()
		: common_shader(nullptr), quadVAO(0), quadVBO(0), bgTextureID(0)
	{
		for (int i = 0; i < 10; ++i) numTextures[i] = 0;
	}

	UIManager::~UIManager() {
		if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
		if (quadVBO) glDeleteBuffers(1, &quadVBO);
	}

	void UIManager::Initialize(Shader* shader) {
		this->common_shader = shader;

		// 1. 배경 로드
		bgTextureID = Core::TextureManager::GetInstance().GetTexture("UI_Back");

		// 2. 숫자 텍스처 ID 캐싱 ("0"~"9")
		for (int i = 0; i <= 9; ++i) {
			numTextures[i] = Core::TextureManager::GetInstance().GetTexture(std::to_string(i));
		}

		ConfigureQuad();
	}

	void UIManager::ConfigureQuad() {
		// 기본 Quad (Pos3 + Normal3 + Tex2)
		float vertices[] = {
			// Pos              // Normal          // Tex
			0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

			0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
			1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
			1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}

	void UIManager::Render(int windowWidth, int windowHeight) {
		if (!common_shader) return;

		// UI 모드 설정
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		common_shader->Activate();
		GLuint prog = common_shader->GetShaderProgramID();
		glUniform1i(glGetUniformLocation(prog, "isUI"), 1);

		// 행렬 설정
		glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
		glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(prog, "normalMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		// 1. 배경
		if (bgTextureID != 0) {
			RenderQuad(bgTextureID, 0, 0, (float)windowWidth, (float)windowHeight);
		}

		// ***************************************************//

		auto& gm = Core::GameManager::GetInstance();
		//int currentScore = gm.GetScore();
		int currentScore = 5678;
		//int highScore = gm.GetHighScore();
		int highScore = 1234;
		const auto& nextFruit = gm.GetNextFruitInfo();

		// **************************************************//


		// 2. 다음 과일 그리기 (개별 이미지 사용)
		float nextSize = 60.0f;



		// 오프셋 조정해서 화면에 맞추기
		float xOffset = 135.0f;
		float yOffset = 165.0f;



		float nextX = windowWidth - 135.0f;
		float nextY = windowHeight - 165.0f;

		// 레벨에 맞는 텍스처 이름 생성 
		std::string texName = "Fruit_" + std::to_string(nextFruit.level);
		GLuint nextFruitTexID = Core::TextureManager::GetInstance().GetTexture(texName);

		// 텍스처가 있으면 그리기
		if (nextFruitTexID != 0) {
			RenderQuad(nextFruitTexID, nextX, nextY, nextSize, nextSize, glm::vec3(1.0f));
		}

		// 3. 점수 그리기



		// 오프셋 조정해서 화면에 맞추기
		float currenScireOffset = 80.0f;
		float highScoreOffset = 120.0f;

		// 글자 크기 조절
		float currenScireScale = 30.0f;
		float highScoreScale = 30.0f;



		RenderNumber(currentScore, 120.0f, windowHeight - currenScireOffset, currenScireScale);
		RenderNumber(highScore, 120.0f, windowHeight - 120.0f, highScoreScale);

		// 복구
		glUniform1i(glGetUniformLocation(prog, "isUI"), 0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void UIManager::RenderQuad(GLuint texID, float x, float y, float w, float h, glm::vec3 color) {
		GLuint prog = common_shader->GetShaderProgramID();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, 0.0f));
		model = glm::scale(model, glm::vec3(w, h, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(prog, "objectColor"), 1, glm::value_ptr(color));

		if (texID != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texID);
			glUniform1i(glGetUniformLocation(prog, "outTexture"), 0);
			glUniform1i(glGetUniformLocation(prog, "useTexture"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(prog, "useTexture"), 0);
		}

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void UIManager::RenderNumber(int number, float x, float y, float scale) {
		std::string s = std::to_string(number);
		float offsetX = 0.0f;

		float spacing = scale * 0.45f;

		for (char c : s) {
			int digit = c - '0';
			GLuint tex = numTextures[digit];

			RenderQuad(tex, x + offsetX, y, scale, scale, glm::vec3(1.0f));

			offsetX += spacing;
		}
	}
}