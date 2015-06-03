#include "Plane.h"




glm::vec3 Plane::getNormal()
{
	return normal;
}


Plane::Plane( glm::vec3 a, glm::vec3 b, glm::vec3 c )
{
	normal = glm::normalize(glm::cross(b - a, c - a));
}

Plane::Plane( glm::vec3 normal, float f )
{
	this->normal = normal;
	this->f = f;
}

float Plane::getOffset()
{
	return f;
}
