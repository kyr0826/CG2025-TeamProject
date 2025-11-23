#include "CollisionUtils.h"

namespace Physics {
	glm::vec3 CollisionUtils::ClosestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		glm::vec3 ab = b - a, ac = c - a, ap = p - a;
		float d1 = glm::dot(ab, ap), d2 = glm::dot(ac, ap);
		if (d1 <= 0.0f && d2 <= 0.0f) return a;

		glm::vec3 bp = p - b;
		float d3 = glm::dot(ab, bp), d4 = glm::dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3) return b;

		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) { return a + ab * (d1 / (d1 - d3)); }

		glm::vec3 cp = p - c;
		float d5 = glm::dot(ab, cp), d6 = glm::dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6) return c;

		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) { return a + ac * (d2 / (d2 - d6)); }

		float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
			return b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));
		}

		float denom = 1.0f / (va + vb + vc);
		return a + ab * (vb * denom) + ac * (vc * denom);
	}

	bool CollisionUtils::IntersectSphereTriangle(const glm::vec3& center, float r, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, ContactInfo& info) {
		glm::vec3 closest = ClosestPointOnTriangle(center, v0, v1, v2);
		glm::vec3 diff = center - closest;
		float distSq = glm::dot(diff, diff);

		if (distSq < r * r) {
			info.isColliding = true;
			float dist = std::sqrt(distSq);
			if (dist < 1e-6f) {
				info.normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
				info.penetrationDepth = r;
			}
			else {
				info.normal = diff / dist;
				info.penetrationDepth = r - dist;
			}
			return true;
		}
		return false;
	}
}
