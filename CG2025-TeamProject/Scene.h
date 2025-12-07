#pragma once

class Scene {
public:
	virtual ~Scene() {}

	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void Reshape(int w, int h) = 0;

	virtual void HandleInput(unsigned char key) = 0;
};