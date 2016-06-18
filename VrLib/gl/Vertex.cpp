#include "Vertex.h"

namespace vrlib
{
	namespace gl
	{
		glm::vec2 encodeNormal(const glm::vec3 &n) {
			float p = glm::sqrt(n.z * 8.0f + 8.0f);
			return glm::vec2(n) / p + 0.5f;
		}

		glm::vec3 decodeNormal(const glm::vec2 &enc) {
			glm::vec2 fenc = enc*4.0f - 2.0f;
			float f = glm::dot(fenc, fenc);
			float g = glm::sqrt(1 - f / 4.0f);
			return glm::vec3(fenc*g, 1.0f - f / 2.0f);
		}

	}
}
