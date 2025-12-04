#include "PhysicsWorld.h"
#include "EventSystem.h"

using namespace Core;

namespace Physics {
	PhysicsWorld::PhysicsWorld() : dish(nullptr) {}

	void PhysicsWorld::AddBall(GameObject* ball) { balls.push_back(ball); }

	void PhysicsWorld::SetDish(GameObject* dish) {
		this->dish = dish;
		BuildDishGrid();
	}

	void PhysicsWorld::Step(float dt) {
		int subSteps = 8;
		float subDt = dt / subSteps;

		int solverIterations = 4;

		for (int s = 0; s < subSteps; ++s) {
			UpdateBallGrid();

			for (auto ball : balls) {
				Physics_Body* body = ball->GetPhysicsBody();

				float r = body->colliderRadius * ball->GetScale().x;
				float maxSpeed = r / subDt * 0.8f;
				if (glm::length(body->vel) > maxSpeed)
					body->vel = glm::normalize(body->vel) * maxSpeed;

				ball->UpdateState(subDt);
				body->UpdateState(subDt);
			}

			for (int iter = 0; iter < solverIterations; ++iter) {
				if (dish) {
					for (auto ball : balls)
						ResolveDishCollision(ball);
				}
				CheckBallCollisions();
			}
		}

		ProcessMerges();
	}

	void PhysicsWorld::BuildDishGrid() {
		if (!dish) return;

		dishTriangles.clear();
		for (int x = 0; x < GRID_SIZE; ++x) {
			for (int z = 0; z < GRID_SIZE; ++z) {
				dishGrid[x][z].clear();
			}
		}

		Model* model = dish->GetModel();
		const std::vector<Vertex> verts = model->GetVertices();
		glm::mat4 trans = GetPhysicsTransform(dish);

		size_t triCount = verts.size() / 3;
		dishTriangles.reserve(triCount);

		for (size_t i = 0; i < triCount; ++i) {
			size_t idx = i * 3;
			glm::vec3 v0 = glm::vec3(trans * glm::vec4(verts[idx + 0].pos, 1.0f));
			glm::vec3 v1 = glm::vec3(trans * glm::vec4(verts[idx + 1].pos, 1.0f));
			glm::vec3 v2 = glm::vec3(trans * glm::vec4(verts[idx + 2].pos, 1.0f));

			glm::vec3 minBox = glm::min(v0, glm::min(v1, v2));
			glm::vec3 maxBox = glm::max(v0, glm::max(v1, v2));

			dishTriangles.push_back({ v0, v1, v2, minBox, maxBox });

			int startX = std::max(0, std::min((int)((minBox.x - WORLD_MIN) / CELL_SIZE), GRID_SIZE - 1));
			int endX = std::max(0, std::min((int)((maxBox.x - WORLD_MIN) / CELL_SIZE), GRID_SIZE - 1));
			int startZ = std::max(0, std::min((int)((minBox.z - WORLD_MIN) / CELL_SIZE), GRID_SIZE - 1));
			int endZ = std::max(0, std::min((int)((maxBox.z - WORLD_MIN) / CELL_SIZE), GRID_SIZE - 1));

			for (int x = startX; x <= endX; ++x) {
				for (int z = startZ; z <= endZ; ++z) {
					dishGrid[x][z].push_back(i);
				}
			}
		}

	}

	void PhysicsWorld::UpdateBallGrid() {
		for (int x = 0; x < GRID_SIZE; ++x) {
			for (int z = 0; z < GRID_SIZE; ++z) {
				ballGrid[x][z].clear();
			}
		}

		for (auto ball : balls) {
			glm::vec3 p = ball->GetPosition();
			int gx = (int)((p.x - WORLD_MIN) / CELL_SIZE);
			int gz = (int)((p.z - WORLD_MIN) / CELL_SIZE);
			if (gx >= 0 && gx < GRID_SIZE &&
				gz >= 0 && gz < GRID_SIZE) {
				ballGrid[gx][gz].push_back(ball);
			}
		}
	}

	void PhysicsWorld::CheckBallCollisions() {
		for (int x = 1; x < GRID_SIZE - 1; ++x) {
			for (int z = 1; z < GRID_SIZE - 1; ++z) {
				auto& cell = ballGrid[x][z];
				if (cell.empty()) continue;

				for (size_t i = 0; i < cell.size(); ++i) {
					for (size_t j = i + 1; j < cell.size(); ++j) {
						ResolveBallCollision(cell[i], cell[j]);

					}
				}

				int dx[] = { 1, 0, 1, -1 };
				int dz[] = { 0, 1, 1, 1 };

				for (int k = 0; k < 4; ++k) {
					auto& neighbor = ballGrid[x + dx[k]][z + dz[k]];

					for (auto b1 : cell) {
						for (auto b2 : neighbor) {
							ResolveBallCollision(b1, b2);
						}
					}
				}
			}
		}
	}

	void PhysicsWorld::ResolveDishCollision(GameObject* ball) {
		Physics_Body* body = ball->GetPhysicsBody();
		float r = body->colliderRadius * ball->GetScale().x;
		glm::vec3 pos = body->pos;

		int centerX = (int)((pos.x - WORLD_MIN) / CELL_SIZE);
		int centerZ = (int)((pos.z - WORLD_MIN) / CELL_SIZE);

		ContactInfo best;
		best.isColliding = false;
		best.penetrationDepth = -1.0f;

		glm::vec3 ballMin = pos - glm::vec3(r);
		glm::vec3 ballMax = pos + glm::vec3(r);

		for (int x = -1; x <= 1; ++x) {
			for (int z = -1; z <= 1; ++z) {
				int checkX = centerX + x;
				int checkZ = centerZ + z;
				if (checkX < 0 || checkX >= GRID_SIZE || checkZ < 0 || checkZ >= GRID_SIZE) continue;

				for (int idx : dishGrid[checkX][checkZ]) {
					TriangleData& tri = dishTriangles[idx];
					if (ballMax.x < tri.min.x || ballMin.x > tri.max.x ||
						ballMax.y < tri.min.y || ballMin.y > tri.max.y ||
						ballMax.z < tri.min.z || ballMin.z > tri.max.z) {
						continue;
					}

					ContactInfo info;
					if (CollisionUtils::IntersectSphereTriangle(body->pos, r, tri.v0, tri.v1, tri.v2, info)) {
						if (glm::dot(info.normal, glm::vec3(0, 1, 0)) < -0.5f) continue;
						if (info.penetrationDepth > best.penetrationDepth) best = info;
					}
				}
			}
		}

		if (best.isColliding) {
			body->pos += best.normal * best.penetrationDepth;
			glm::vec3 vn = glm::dot(body->vel, best.normal) * best.normal;
			glm::vec3 vt = body->vel - vn;
			if (glm::dot(body->vel, best.normal) < 0.f)
				body->vel = (vt * 0.99f) - (vn * 0.0f);
		}
	}

	void PhysicsWorld::ResolveBallCollision(GameObject* b1, GameObject* b2) {
		if (std::find(pendingRemoval.begin(), pendingRemoval.end(), b1) != pendingRemoval.end()) return;
		if (std::find(pendingRemoval.begin(), pendingRemoval.end(), b2) != pendingRemoval.end()) return;

		auto p1 = b1->GetPhysicsBody();
		auto p2 = b2->GetPhysicsBody();

		float r1 = p1->colliderRadius * b1->GetScale().x;
		float r2 = p2->colliderRadius * b2->GetScale().x;

		float dist = glm::distance(p1->pos, p2->pos);
		float overlap = (r1 + r2) - dist;

		if (overlap > 0.0f) {
			if (b1->fruitLevel != -1 && b1->fruitLevel == b2->fruitLevel) {
				// 마지막 레벨(수박)은 합쳐지지 않음 (보통 사라지게 하거나 유지)
				if (b1->fruitLevel < 10) {
					// 병합 이벤트 등록
					MergeEvent ev;
					ev.a = b1;
					ev.b = b2;

					glm::vec3 dir = glm::normalize(p2->pos - p1->pos);
					ev.position = p1->pos + (dir * r1);
					ev.nextLevel = b1->fruitLevel + 1;

					mergeQueue.push_back(ev);
					pendingRemoval.push_back(b1);
					pendingRemoval.push_back(b2);

					// 1. 점수: (다음 레벨 + 1) * 10점
					int scoreToAdd = (ev.nextLevel + 1) * 10;
					
					EventSystem::GetInstance().PublishScore(scoreToAdd);

					// 2. 사운드: 팝 효과음 재생 요청
					EventSystem::GetInstance().PublishSound("Merge");

					// 3. 수박(레벨 10) 완성 시 게임 클리어/종료 처리 예시
					if (ev.nextLevel == 10) {

					}
					return;
				}
			}

			glm::vec3 n = (dist < 1e-4f) ? glm::vec3(0, 1, 0) : glm::normalize(p1->pos - p2->pos);
			float invM = p1->mass_inv + p2->mass_inv;

			glm::vec3 relVel = p1->vel - p2->vel;
			float vn = glm::dot(relVel, n);
			if (vn > 0.0f) return;

			float restitution = 0.01f;
			float j = -(1.0f + restitution) * vn / invM;
			glm::vec3 imp = j * n;
			p1->vel += imp * p1->mass_inv;
			p2->vel -= imp * p2->mass_inv;

			glm::vec3 t = relVel - n * vn;
			if (glm::length(t) > 1e-4f) {
				t = glm::normalize(t);
				float jt = -glm::dot(relVel, t) / invM;
				float maxF = j * 0.2f;
				if (std::abs(jt) > maxF)
					jt = maxF * (jt > 0 ? 1 : -1);

				glm::vec3 fImp = t * jt;
				p1->vel += fImp * p1->mass_inv;
				p2->vel -= fImp * p2->mass_inv;
			}
			float percent = 0.9f;
			float slop = 0.001f; // 허용 침투 깊이 (이만큼은 겹쳐도 봐줌)

			float correctionMag = std::max(overlap - slop, 0.0f) / invM * percent;

			glm::vec3 correction = n * correctionMag;
			p1->pos += correction * p1->mass_inv;
			p2->pos -= correction * p2->mass_inv;
		}
	}

	glm::mat4 PhysicsWorld::GetPhysicsTransform(GameObject* obj) {
		glm::mat4 T = glm::translate(glm::mat4(1.0f), obj->GetPosition());
		glm::mat4 R = glm::mat4(1.0f);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), obj->GetScale());

		return T * R * S;
	}

	void PhysicsWorld::ProcessMerges() {
		if (onMerge) {
			for (const auto& ev : mergeQueue) {
				onMerge(ev.nextLevel, ev.position);
			}
		}
		mergeQueue.clear();

		if (!pendingRemoval.empty()) {
			for (auto obj : pendingRemoval) {
				// [수정] erase-remove idiom을 정확하게 사용하여 물리 리스트에서 영구 제거
				balls.erase(std::remove(balls.begin(), balls.end(), obj), balls.end());

				// 메인(렌더링) 쪽에 삭제 요청
				if (onRemove) onRemove(obj);
			}
			pendingRemoval.clear();
		}
	}

	void PhysicsWorld::RemoveObject(GameObject* obj) {
		auto it = std::remove(balls.begin(), balls.end(), obj);
		if (it != balls.end()) {
			balls.erase(it, balls.end());
		}
	}
}
