#pragma once
#include <vector>
#include <memory>

#include "Scene.h"

#include "GameObject.h"
#include "PhysicsWorld.h"
#include "Camera.h"
#include "GameManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "EventSystem.h"
#include "TitleScene.h"
#include "UIManager.h"
#include "ShaderManager.h"

class GameScene : public Scene {
public:
	Graphics::Shader* common_shader;
	Graphics::Shader* trajectoryShader;
	Graphics::Camera* main_camera;
	std::shared_ptr<Physics::PhysicsWorld> physicsWorld;

	std::vector<Objects::GameObject*> renderObjects;

	Model* common_sphere_model = nullptr;
	float common_sphere_radius = 0.0f;
	Model* dish_model = nullptr;
	Model* bg_cube_model = nullptr;

	float g_launch_pitch = 54.0f;			// 발사 각도 (높게 던져야 포물선이 예쁨)
	float g_launch_yaw = 0.0f;				// 카메라 공전 각도
	float g_launch_force = LAUNCH_FORCE;	// 발사 힘 (적당히 멀리 날아가도록)
	float g_spawn_timer = 0.0f;				// 다음 과일 생성까지의 시간 카운터

	GameObject* readyFruit = nullptr;

	// 궤적 렌더링용
	GLuint g_trajVAO = 0;
	GLuint g_trajVBO = 0;

	std::vector<glm::vec3> trajectoryPoints;

	bool isEnd;

	GameScene() :
		common_shader(nullptr),
		trajectoryShader(nullptr),
		main_camera(nullptr),
		physicsWorld(nullptr),
		isEnd(false) {
	}

	void OnEnter() override {
		std::cout << "GameScene Enter" << std::endl;
		Core::SoundManager::GetInstance().StopBGM();
		Core::SoundManager::GetInstance().Play("InGame");
		common_shader = ShaderManager::GetInstance().GetShader("common_shader");

		main_camera = Core::GameManager::GetInstance().GetGameCamera();

		physicsWorld = std::make_shared< Physics::PhysicsWorld>();
		physicsWorld->SetMergeCallback(
			std::bind(&GameScene::OnFruitMerge, this, std::placeholders::_1, std::placeholders::_2));
		physicsWorld->SetRemoveCallback(
			std::bind(&GameScene::OnObjectRemove, this, std::placeholders::_1));

		trajectoryShader = ShaderManager::GetInstance().GetShader("trajectory_shader");
		glGenVertexArrays(1, &g_trajVAO);
		glGenBuffers(1, &g_trajVBO);

		InitGame();
	}
	void OnExit() override {
		std::cout << "GameScene Exit" << std::endl;
		/*for (auto* obj : renderObjects)
			delete obj;*/

		renderObjects.clear();

		//delete physicsWorld;
	}

	void Update(float deltaTime) override {
		if (Core::GameManager::GetInstance().GetCurrentGameState() == Core::GameState::GameOver) return;

		if (g_spawn_timer > 0.0f) {
			g_spawn_timer -= 0.016f;
			if (g_spawn_timer < 0.0f) {
				g_spawn_timer = 0.0f;
				SpawnReadyFruit();
			}
		}

		physicsWorld->Step(deltaTime);

		main_camera->Position.x = sin(glm::radians(g_launch_yaw)) * CAMERA_DISTANCE;
		main_camera->Position.z = cos(glm::radians(g_launch_yaw)) * CAMERA_DISTANCE;

		if (readyFruit && g_spawn_timer == 0.0f) {
			glm::vec3 launchPos = CalculateLaunchPosition();
			readyFruit->SetPosition(launchPos);

			// 대기 중엔 물리 영향 안 받음
			readyFruit->GetPhysicsBody()->vel = glm::vec3(0);
			readyFruit->SetAcceleration(glm::vec3(0));

			// 궤적 계산
			CalculateTrajectory();
		}
	}

	void Reshape(int w, int h) override {
		main_camera->width = w;
		main_camera->height = h;
	}

	void Render() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		for (auto obj : renderObjects)
			obj->RenderModel(*main_camera);

		DrawTrajectory();

		Graphics::UIManager::GetInstance().RenderGameScene(main_camera->width, main_camera->height);
	}

	void HandleInput(unsigned char key) override {
		switch (key) {
		case KEY_ESC:
			Core::GameManager::GetInstance().GameOver();
			break;

		case KEY_SPACE: // Space: 발사
		{
			if (Core::GameManager::GetInstance().GetCurrentGameState() == Core::GameState::GameOver) return;
			if (readyFruit) {
				Physics_Body* body = readyFruit->GetPhysicsBody();
				body->vel = CalculateLaunchVelocity();
				readyFruit->SetBillboard(false);
				readyFruit->SetAcceleration(glm::vec3(0, -GRAVITY, 0));

				physicsWorld->AddBall(readyFruit);
				g_spawn_timer = SPAWN_DELAY;
				readyFruit = nullptr;
				Core::GameManager::GetInstance().UpdateFruit();
			}
		}
		break;

		case 'a':
		case 'A':
		case 'd':
		case 'D':
			if (Core::GameManager::GetInstance().GetCurrentGameState() == Core::GameState::GameOver) return;
			g_launch_yaw += ((key == 'a' || key == 'A') ? -1 : 1) * CAMERA_ROTATION_SPEED;
			Core::SoundManager::GetInstance().Play("Rotate");
			break;

		case 'w':
		case 'W':
			if (Core::GameManager::GetInstance().GetCurrentGameState() == Core::GameState::GameOver) return;
			g_launch_pitch = std::min(g_launch_pitch + 2.0f, 68.0f);
			Core::SoundManager::GetInstance().Play("Rotate");
			break;

		case 's':
		case 'S':
			if (Core::GameManager::GetInstance().GetCurrentGameState() == Core::GameState::GameOver) return;
			g_launch_pitch = std::max(g_launch_pitch - 2.0f, 14.0f);
			Core::SoundManager::GetInstance().Play("Rotate");
			break;
		}
	}

	void SpawnReadyFruit() {
		Core::FruitInfo info = Core::GameManager::GetInstance().CurFruit();

		Physics_Body* body = new Physics_Body(info.mass);
		body->colliderRadius = common_sphere_radius;

		readyFruit = new GameObject(info.name, body, common_sphere_model, common_shader);

		readyFruit->SetScale(glm::vec3(info.scale));
		readyFruit->SetTexture(Core::TextureManager::GetInstance().GetTexture(info.name));
		readyFruit->SetModelColor(info.color);
		readyFruit->fruitLevel = info.level;
		readyFruit->SetBillboard(true);
		renderObjects.push_back(readyFruit);
	}

private:
	void InitGame() {
		common_sphere_model = new Model("Fruit_Model.obj");
		common_sphere_model->Recenter();
		common_sphere_radius = common_sphere_model->GetExactRadius() * 0.98f;

		dish_model = new Model("Dish.obj");
		dish_model->Recenter();

		bg_cube_model = new Model("GameBG_Cube.obj");
		bg_cube_model->Recenter();

		GameObject* bg_cube = new GameObject("BG_Cube", new Physics_Body(0.0f), bg_cube_model, common_shader);
		bg_cube->SetScale(glm::vec3(10.0f));
		bg_cube->SetTexture(Core::TextureManager::GetInstance().GetTexture("BG_Cube"));
		renderObjects.push_back(bg_cube);

		CreateDish();
		SpawnReadyFruit();

		Core::GameManager::GetInstance().GameStart();
	}

	void ResetGame() {
	}

	void OnFruitMerge(int nextLevel, glm::vec3 pos) {
		const Core::FruitInfo& info = Core::GameManager::GetInstance().GetFruitInfo(nextLevel);

		Physics_Body* body = new Physics_Body(info.mass);
		body->colliderRadius = common_sphere_radius * 0.95f;

		body->pos = pos;
		body->vel = glm::vec3(0.0f);
		body->acc = glm::vec3(0, -GRAVITY, 0);

		GameObject* newFruit = new GameObject(info.name, body, common_sphere_model, common_shader);
		newFruit->SetScale(glm::vec3(info.scale));
		newFruit->SetModelColor(info.color);
		newFruit->SetTexture(Core::TextureManager::GetInstance().GetTexture(info.name));
		newFruit->fruitLevel = nextLevel;
		newFruit->PlayMergeAnimation();

		physicsWorld->AddBall(newFruit);
		renderObjects.push_back(newFruit);
		printf("Merge! Level %d -> %d\n", nextLevel - 1, nextLevel);
	}

	void OnObjectRemove(GameObject* obj) {
		auto it = std::remove(renderObjects.begin(), renderObjects.end(), obj);
		renderObjects.erase(it, renderObjects.end());

		//delete obj; // 메모리 해제
	}

	glm::vec3 CalculateLaunchPosition() {
		glm::vec3 camPos = main_camera->Position;

		glm::vec3 target = glm::vec3(0, 0, 0);
		glm::vec3 forward = glm::normalize(target - camPos);
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
		glm::vec3 up = glm::cross(right, forward);

		glm::vec3 offset =
			(forward * READY_FRUIT_OFFSET.z) +
			(right * READY_FRUIT_OFFSET.x) +
			(up * READY_FRUIT_OFFSET.y);

		return camPos + offset;
	}

	glm::vec3 CalculateLaunchVelocity() {
		glm::vec3 launchPos = CalculateLaunchPosition();
		glm::vec3 target = glm::vec3(0, 0, 0);

		glm::vec3 dir = glm::normalize(target - launchPos);
		glm::vec3 horizontalDir = glm::normalize(glm::vec3(dir.x, 0, dir.z));

		float radPitch = glm::radians(g_launch_pitch);

		// 위쪽으로 쏘는 벡터 (Pitch 적용)
		glm::vec3 launchDir = horizontalDir * cos(radPitch) + glm::vec3(0, 1, 0) * sin(radPitch);

		return glm::normalize(launchDir) * g_launch_force;
	}

	void CalculateTrajectory() {
		trajectoryPoints.clear();
		glm::vec3 startPos;
		if (readyFruit) startPos = readyFruit->GetPosition();
		else startPos = CalculateLaunchPosition();

		glm::vec3 vel = CalculateLaunchVelocity();
		glm::vec3 cur = startPos;
		trajectoryPoints.push_back(cur);

		for (int i = 0; i < TRAJECTORY_POINTS_COUNT; ++i) {
			vel += glm::vec3(0, -GRAVITY, 0) * TRAJECTORY_TIME_STEP;
			cur += vel * TRAJECTORY_TIME_STEP;
			trajectoryPoints.push_back(cur);
			if (cur.y < -2.0f) break;
		}
	}

	void DrawTrajectory() {
		if (trajectoryPoints.size() < 2) return;
		if (g_spawn_timer > 0.0f) return;

		trajectoryShader->Activate();

		glm::mat4 view = glm::lookAt(
			main_camera->Position,
			glm::vec3(0, 0, 0),
			main_camera->Up
		);

		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			(float)main_camera->width / (float)main_camera->height,
			0.1f,
			100.0f
		);

		trajectoryShader->SetMat4("view", view);
		trajectoryShader->SetMat4("projection", proj);
		trajectoryShader->SetVec3("lineColor", glm::vec3(0.2, 0.9f, 1.0f));

		glBindVertexArray(g_trajVAO);
		glBindBuffer(GL_ARRAY_BUFFER, g_trajVBO);

		glBufferData(GL_ARRAY_BUFFER, trajectoryPoints.size() * sizeof(glm::vec3), trajectoryPoints.data(), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glLineWidth(4.0f);
		glDrawArrays(GL_LINE_STRIP, 0, trajectoryPoints.size());
		glLineWidth(1.0f);

		glBindVertexArray(0);
	}

	void CreateDish() {
		GameObject* dish = new GameObject("Dish", new Physics_Body(0.0f), dish_model, common_shader);
		dish->SetPosition(glm::vec3(0, DISH_Y_OFFSET, 0));
		dish->SetScale(glm::vec3(DISH_SCALE));

		GLuint dishTextureID = Core::TextureManager::GetInstance().GetTexture("Dish");
		if (dishTextureID != 0) { dish->SetTexture(dishTextureID); }

		dish->SetModelColor(glm::vec3(1.0f, 1.0f, 1.0f));

		physicsWorld->SetDish(dish);
		renderObjects.push_back(dish);
	}
};