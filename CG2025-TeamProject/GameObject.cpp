#include "GameObject.h"

namespace Objects {
	GameObject::GameObject(std::string name, Physics_Body* physics, Model* model, Shader* shader) :
		name{ name }, physics{ physics }, model{ model }, shader{ shader } {
		model_scale = glm::vec3(1.0f);
		model_color = glm::vec3(.85f);
		model->SetShader(shader);
	}

	GameObject::~GameObject() {
		if (physics) 
			delete physics;
	}

	void GameObject::RenderModel(Camera& camera) {
		shader->Activate();
		glUniform3fv(glGetUniformLocation(shader->GetShaderProgramID(), "objectColor"), 1, glm::value_ptr(model_color));

		glm::mat4 T = glm::translate(glm::mat4(1.0f), physics->pos);
		glm::mat4 R = glm::mat4(1.0f);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), model_scale);

		// OpenGL = T R S ¸ðµ¨ ( ¼ø¼­ : <- )
		model->SetModelMatrix(T * R * S);
		camera.Matrix(45.0f, 0.1f, 100.0f, *shader);
		model->Render();
	}
}