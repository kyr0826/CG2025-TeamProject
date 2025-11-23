#pragma once
#include "GameObject.h"
#include "CollisionUtils.h"
#include "CommonUtils.h"

using namespace Objects;

namespace Physics {
	struct TriangleData {
		glm::vec3 v0, v1, v2;
		glm::vec3 min, max;
	};

	struct MergeEvent {
		GameObject* a;
		GameObject* b;
		glm::vec3 position;
		int nextLevel;
	};

	class PhysicsWorld {
	public:
		PhysicsWorld();
		void AddBall(GameObject* ball);
		void SetDish(GameObject* dish);
		void Step(float dt);

		using MergeCallback = void(*)(int level, glm::vec3 pos);
		void SetMergeCallback(MergeCallback callback) { onMerge = callback; }

		using RemoveCallback = void(*)(GameObject* obj);
		void SetRemoveCallback(RemoveCallback callback) { onRemove = callback; }
	private:
		std::vector<GameObject*> balls;
		GameObject* dish;

		std::vector<MergeEvent> mergeQueue;
		std::vector<GameObject*> pendingRemoval;

		MergeCallback onMerge = nullptr;
		RemoveCallback onRemove = nullptr;

		static const int GRID_SIZE = 40;
		const float WORLD_MIN = -20.0f;
		const float CELL_SIZE = 40.0f / GRID_SIZE;

		std::vector<TriangleData> dishTriangles;
		std::vector<int> dishGrid[GRID_SIZE][GRID_SIZE];

		std::vector<GameObject*> ballGrid[GRID_SIZE][GRID_SIZE];

		void BuildDishGrid();
		void UpdateBallGrid();
		void CheckBallCollisions();
		void ResolveDishCollision(GameObject* ball);
		void ResolveBallCollision(GameObject* b1, GameObject* b2);
		glm::mat4 GetPhysicsTransform(GameObject* obj);

		void ProcessMerges();
		void RemoveObject(GameObject* obj);
	};
}