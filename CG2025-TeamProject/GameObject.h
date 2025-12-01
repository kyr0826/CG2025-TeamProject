#pragma once
#include <cmath>
#include "Physics_Body.h"
#include "Model.h"
#include "Camera.h"

using namespace Physics;
using namespace Graphics;

namespace Objects {
	class GameObject {
	public:
		GameObject(std::string name, Physics_Body* physics, Model* model, Shader* shader);
		~GameObject();

		int fruitLevel = -1;

		Physics_Body* GetPhysicsBody() { return physics; }
		Model* GetModel() { return model; }

		glm::vec3 GetPosition() { return physics->pos; }
		glm::vec3 GetScale() { return model_scale; }

		void SetPosition(const glm::vec3& pos) { physics->pos = pos; }
		void SetScale(const glm::vec3& scale) { 
			model_scale = scale; 
			origin_model_scale = model_scale;
		}
		void SetAcceleration(const glm::vec3& acc) { physics->acc = acc; }
		void SetModelColor(const glm::vec3& color) { model_color = color; }

		void SetTexture(GLuint texID) { textureID = texID; }

		void RenderModel(Camera& camera);

		void PlayMergeAnimation();

		void UpdateState(float dt);

	private:
		Physics_Body* physics;
		Model* model;
		Shader* shader;
		std::string name;
		glm::vec3 model_scale;
		glm::vec3 origin_model_scale;
		glm::vec3 model_color;

		GLuint textureID = 0;

		float animate_time{};
	};
}