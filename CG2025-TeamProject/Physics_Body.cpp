#include "Physics_Body.h"
#include "GameManager.h"

using namespace Core;

namespace Physics {
	Physics_Body::Physics_Body(float mass) {
		this->mass = mass;
		this->mass_inv = (mass != 0.0f) ? 1.0f / mass : 0.0f;

		pos = glm::vec3(0.0f);
		vel = glm::vec3(0.0f);
		acc = glm::vec3(0.0f);
		colliderRadius = 0.5f;
	}

	void Physics_Body::UpdateState(float dt) {
		if (mass_inv == 0.0f) return;

		vel += acc * dt;
		pos += vel * dt;

		if (pos.y < -3.0f) {
			GameManager::GetInstance().GameOver();
		}
	}
}
