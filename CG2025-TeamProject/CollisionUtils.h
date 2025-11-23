#pragma once
#include "Common_Headers.h"
namespace Physics {
	struct ContactInfo {
		bool isColliding;
		glm::vec3 contactPoint, normal;
		float penetrationDepth;
	};

	class CollisionUtils {
	public:
		static glm::vec3 ClosestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
		static bool IntersectSphereTriangle(const glm::vec3& center, float r, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, ContactInfo& info);
	};
}