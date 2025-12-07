#pragma once
#include <string>
#include <vector>
#include "Scene.h"
#include "Common_Headers.h"
#include "SoundManager.h"
#include "TextureManager.h"

class LoadingScene : public Scene {
public:
	LoadingScene();

	void OnEnter() override;
	void OnExit() override;

	void Update(float deltaTime) override;

	void Reshape(int w, int h) override;
	void Render() override;

	void HandleInput(unsigned char key) override;

private:
	std::vector<Core::TextureInfo> pendingTextures;
	std::vector<Core::SoundInfo> pendingSounds;
	int textureLoadIndex;
	int soundLoadIndex;

	GLuint loadingTexture;

	int width, height;
};