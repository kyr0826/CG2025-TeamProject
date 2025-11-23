#pragma once
#include "Common_Headers.h"

namespace Physics {
	class Physics_Body {
	public:
		Physics_Body(float mass);
		void UpdateState(float dt);

		float mass, mass_inv;
		glm::vec3 pos, vel, acc;
		float colliderRadius;
	};
}

