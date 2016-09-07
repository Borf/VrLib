#include "Frustum.h"

#include <glm/gtc/matrix_access.hpp>


namespace vrlib
{
	namespace math
	{
		Frustum::Frustum(const glm::mat4 & projectionMatrix, const glm::mat4 & modelviewMatrix)
		{ // http://www.lighthouse3d.com/tutorials/view-frustum-culling/clip-space-approach-extracting-the-planes/
			// Extracting the planes.
			glm::mat4 matrix = projectionMatrix * modelviewMatrix;

			glm::vec4 rowX = glm::row(matrix, 0);
			glm::vec4 rowY = glm::row(matrix, 1);
			glm::vec4 rowZ = glm::row(matrix, 2);
			glm::vec4 rowW = glm::row(matrix, 3);

			planes[0] = glm::normalize(rowW + rowX);
			planes[1] = glm::normalize(rowW - rowX);
			planes[2] = glm::normalize(rowW + rowY);
			planes[3] = glm::normalize(rowW - rowY);
			planes[4] = glm::normalize(rowW + rowZ);
			planes[5] = glm::normalize(rowW - rowZ);


			// Normalizing the planes.
			for (int i = 0; i < 4; i++)
				planes[i] = -planes[i] / glm::length(glm::vec3(planes[i]));
		}
	}
}