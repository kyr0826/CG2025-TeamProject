#include "UIManager.h"
#include "ShaderManager.h"

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

	void UIManager::Initialize() {
		this->common_shader = ShaderManager::GetInstance().GetShader("common_shader");

		//// 1. 배경 로드
		//bgTextureID = Core::TextureManager::GetInstance().GetTexture("UI_Back");

		//// 2. 숫자 텍스처 ID 캐싱 ("0"~"9")
		//for (int i = 0; i <= 9; ++i) {
		//	numTextures[i] = Core::TextureManager::GetInstance().GetTexture(std::to_string(i));
		//}

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

	void UIManager::RenderLoadingScene(int windowWidth, int windowHeight, int percent, int text_x, int text_y, float text_scale) {
		if (!common_shader) return;
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		common_shader->Activate();
		GLuint prog = common_shader->GetShaderProgramID();
		glUniform1i(glGetUniformLocation(prog, "isUI"), 1);

		glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
		glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(prog, "normalMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		GLuint bgTextureID = Core::TextureManager::GetInstance().GetTexture("Loading");
		if (bgTextureID != 0) {
			RenderQuad(bgTextureID, 0, 0, (float)windowWidth, (float)windowHeight);
		}

		RenderNumber(percent, text_x, text_y, text_scale);

		glUniform1i(glGetUniformLocation(prog, "isUI"), 0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void UIManager::RenderTitleScene(int windowWidth, int windowHeight) {
		if (!common_shader) return;
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		common_shader->Activate();
		GLuint prog = common_shader->GetShaderProgramID();
		glUniform1i(glGetUniformLocation(prog, "isUI"), 1);

		glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
		glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(prog, "normalMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		GLuint bgTextureID = Core::TextureManager::GetInstance().GetTexture("Title");
		if (bgTextureID != 0) {
			RenderQuad(bgTextureID, 0, 0, (float)windowWidth, (float)windowHeight);
		}

		glUniform1i(glGetUniformLocation(prog, "isUI"), 0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void UIManager::RenderGameScene(int windowWidth, int windowHeight) {
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

		GLuint bgTextureID = Core::TextureManager::GetInstance().GetTexture("InGame");
		if (bgTextureID != 0) {
			RenderQuad(bgTextureID, 0, 0, (float)windowWidth, (float)windowHeight);
		}

		// ***************************************************//
		auto& gm = Core::GameManager::GetInstance();
		int currentScore = gm.GetScore();
		int highScore = gm.GetHighScore();
		const auto& nextFruit = gm.NextFruit();
		// **************************************************//


		// 2. 다음 과일 그리기 (개별 이미지 사용)
		float nextSize = 100.0f;

		// 오프셋 조정해서 화면에 맞추기
		float xOffset = windowWidth * 0.1796875f;
		float yOffset = windowHeight * 0.277777777777f;

		float nextX = windowWidth - xOffset;
		float nextY = windowHeight - yOffset;

		// 레벨에 맞는 텍스처 이름 생성 
		std::string texName = "Fruit_" + std::to_string(nextFruit.level);
		GLuint nextFruitTexID = Core::TextureManager::GetInstance().GetTexture(texName);

		// 텍스처가 있으면 그리기
		if (nextFruitTexID != 0) {
			RenderQuad(nextFruitTexID, nextX, nextY, nextSize, nextSize, glm::vec3(1.0f), -12.0f);
		}

		// 3. 점수 그리기
		// 오프셋 조정해서 화면에 맞추기
		float currenScore_X_Offset = 160.0f;
		float highScore_X_Offset = currenScore_X_Offset;

		float currentScore_Y_Offset = -100.0f;
		float highScore_Y_Offset = currentScore_Y_Offset - 50.0f;

		// 글자 크기 조절
		float currenScireScale = 40.0f;
		float highScoreScale = 40.0f;

		RenderNumber(currentScore, currenScore_X_Offset, windowHeight + currentScore_Y_Offset, currenScireScale);
		RenderNumber(highScore, highScore_X_Offset, windowHeight + highScore_Y_Offset, highScoreScale);

		// 복구
		glUniform1i(glGetUniformLocation(prog, "isUI"), 0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void UIManager::RenderGameOverScene(int windowWidth, int windowHeight) {
		if (!common_shader) return;
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		common_shader->Activate();
		GLuint prog = common_shader->GetShaderProgramID();
		glUniform1i(glGetUniformLocation(prog, "isUI"), 1);

		glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
		glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(prog, "normalMat"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		GLuint bgTextureID = Core::TextureManager::GetInstance().GetTexture("GameOver");
		if (bgTextureID != 0) {
			RenderQuad(bgTextureID, 0, 0, (float)windowWidth, (float)windowHeight);
		}

		auto& gm = Core::GameManager::GetInstance();
		int currentScore = gm.GetScore();
		int highScore = gm.GetHighScore();

		float currenScore_X_Offset = -100.0f;
		float highScore_X_Offset = currenScore_X_Offset;

		float currentScore_Y_Offset = 118.0f;
		float highScore_Y_Offset = currentScore_Y_Offset - 115.0f;

		// 글자 크기 조절
		float currenScireScale = 120.0f;
		float highScoreScale = 120.0f;

		RenderNumber(currentScore, windowWidth * 0.5f + currenScore_X_Offset, windowHeight * 0.5f + currentScore_Y_Offset, currenScireScale);
		RenderNumber(highScore, windowWidth * 0.5f + highScore_X_Offset, windowHeight * 0.5f + highScore_Y_Offset, highScoreScale);

		glUniform1i(glGetUniformLocation(prog, "isUI"), 0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void UIManager::RenderQuad(GLuint texID, float x, float y, float w, float h, glm::vec3 color, float rot) {
		GLuint prog = common_shader->GetShaderProgramID();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, 0.0f));
		model = glm::rotate(model, glm::radians(rot), glm::vec3(0, 0, 1.0f));
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
			GLuint tex = Core::TextureManager::GetInstance().GetTexture(std::to_string(digit));
			if (tex == 0) continue;

			RenderQuad(tex, x + offsetX, y, scale, scale, glm::vec3(1.0f));
			offsetX += spacing;
		}
	}
}