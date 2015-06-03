#pragma once

#include <glm/glm.hpp>

#pragma warning (disable: 4251)

class Plane
{
	glm::vec3 normal;
	float f;
public:
	Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c);
	Plane(glm::vec3 normal, float f);

	glm::vec3 getNormal();
	float		getOffset();

};