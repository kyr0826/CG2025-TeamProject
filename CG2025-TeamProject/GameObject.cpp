#include "GameObject.h"

namespace Objects {
	GameObject::GameObject(std::string name, Physics_Body* physics, Model* model, Shader* shader) :
		name{ name }, physics{ physics }, model{ model }, shader{ shader } {
		model_scale = glm::vec3(1.0f);
		origin_model_scale = model_scale;
		model_color = glm::vec3(.85f);

		textureID = 0;

		model->SetShader(shader);
	}

	GameObject::~GameObject() {
		if (physics) 
			delete physics;
	}

	void GameObject::RenderModel(Camera& camera) {
		shader->Activate();
		GLuint id = shader->GetShaderProgramID();

		glUniform3fv(glGetUniformLocation(shader->GetShaderProgramID(), "objectColor"), 1, glm::value_ptr(model_color));

		if (this->textureID != 0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(glGetUniformLocation(id, "outTexture"), 0); // 0번 슬롯
		}

		glUniform1i(glGetUniformLocation(id, "useTexture"), this->textureID);

		glm::mat4 T = glm::translate(glm::mat4(1.0f), physics->pos);
		glm::mat4 R = glm::mat4(1.0f);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), model_scale);

		// OpenGL = T R S 모델 ( 순서 : <- )
		model->SetModelMatrix(T * R * S);
		camera.Matrix(45.0f, 0.1f, 100.0f, *shader);
		model->Render();
	}

	void GameObject::PlayMergeAnimation() {
		animate_time = MERGE_ANIMATION_TIME;
	}

	void GameObject::UpdateState(float dt) {
		if (animate_time > 0.0f) {
			animate_time -= dt;
			if (animate_time < 0.0f) {
				animate_time = 0.0f;
				model_scale = origin_model_scale;
			}

			//     t : 0	-> 0.5	-> 1
			// scale : 0.25 -> 1.25 -> 1
			float t = std::min(1.0f,1.0f - (animate_time / MERGE_ANIMATION_TIME));
			float scaleFactor = 0.25f + glm::sin(t * ((M_PI / 2.0f) * 1.45f));
			model_scale = origin_model_scale * scaleFactor;
		}
	}
}