#pragma once
#include "Scene.h"

class GameOverScene : public Scene {
public:
	void OnEnter() override;

	void OnExit() override;

	void Update(float deltaTime) override;

	void Reshape(int w, int h) override;

	void Render() override;

	void HandleInput(unsigned char key) override;

private:
};