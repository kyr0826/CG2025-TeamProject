#include "LoadingScene.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "TitleScene.h"

LoadingScene::LoadingScene() : textureLoadIndex(0), soundLoadIndex(0), loadingTexture(0), width(WINDOW_WIDTH), height(WINDOW_HEIGHT) {}

void LoadingScene::OnEnter() {
	Core::TextureManager::GetInstance().LoadTexture("Loading", (TEXTURE_ROOT + "/Loading_Scene.png").c_str());
	loadingTexture = Core::TextureManager::GetInstance().GetTexture("Loading");

	pendingTextures = {
		{"Dish",		TEXTURE_ROOT + "/Dish_Texture.png"},
		{"Blueberry",	TEXTURE_ROOT + "/Blueberry_Texture.png"},
		{"Cherry",		TEXTURE_ROOT + "/Cherry_Texture.png"},
		{"Plum",		TEXTURE_ROOT + "/Plum_Texture.png"},
		{"Mandarin",	TEXTURE_ROOT + "/Mandarin_Texture.png"},
		{"Persimmon",	TEXTURE_ROOT + "/Persimmon_Texture.png"},
		{"Apple",		TEXTURE_ROOT + "/Apple_Texture.png"},
		{"Pear",		TEXTURE_ROOT + "/Pear_Texture.png"},
		{"Peach",		TEXTURE_ROOT + "/Peach_Texture.png"},
		{"Papaya",		TEXTURE_ROOT + "/Papaya_Texture.png"},
		{"Melon",		TEXTURE_ROOT + "/Melon_Texture.png"},
		{"Watermelon",	TEXTURE_ROOT + "/Watermelon_Texture.png"},
		{"Title",		TEXTURE_ROOT + "/Title_BG.png"},
		{"InGame",		TEXTURE_ROOT + "/InGame_BG.png"},
		{"GameOver",		TEXTURE_ROOT + "/GameOver_BG.png"},
	};

	for (int i = 0; i <= 9; ++i) {
		pendingTextures.push_back({ std::to_string(i), TEXTURE_ROOT + "/" + std::to_string(i) + ".png" });
	}

	for (int i = 0; i <= 10; ++i) {
		std::string name = "Fruit_" + std::to_string(i);
		pendingTextures.push_back({ name, TEXTURE_ROOT + "/" + name + ".png" });
	}

	pendingSounds = {
		{"Title",		SOUND_ROOT + "/Title_BGM.mp3", true},
		{"InGame",		SOUND_ROOT + "/InGame_BGM.mp3", true},
		{"Merge",		SOUND_ROOT + "/Merge.wav", false},
		{"Rotate",		SOUND_ROOT + "/Rotate.wav", false},
		{"GameOver",	SOUND_ROOT + "/GameOver.wav", false},
	};
}

void LoadingScene::OnExit() {}

void LoadingScene::Update(float deltaTime) {
	if (textureLoadIndex < pendingTextures.size()) {
		auto& item = pendingTextures[textureLoadIndex];
		Core::TextureManager::GetInstance().LoadTexture(item.name, item.path.c_str());
		textureLoadIndex++;
		return;
	}

	if (soundLoadIndex < pendingSounds.size()) {
		auto& item = pendingSounds[soundLoadIndex];
		Core::SoundManager::GetInstance().LoadSound(item.name, item.path, item.isBGM);
		soundLoadIndex++;
		return;
	}

	SceneManager::GetInstance().ChangeScene(std::make_shared<TitleScene>());
}

void LoadingScene::Reshape(int w, int h) {
	width = w;
	height = h;
}

void LoadingScene::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Graphics::UIManager& ui = Graphics::UIManager::GetInstance();

	float percent = (float)(textureLoadIndex + soundLoadIndex) / (pendingTextures.size() + pendingSounds.size());
	float x_offset = -78.0f;
	float y_offset = -46.0f;
	ui.RenderLoadingScene(width, height, (int)(percent * 100), width * 0.5f + x_offset, height * 0.5f + y_offset, 100.0f);
}

void LoadingScene::HandleInput(unsigned char key) {}