#include "Ray.h"
#include "Plane.h"

namespace vrlib
{
	namespace math
	{
		Ray::Ray(const glm::vec3 &origin, const glm::vec3 &dir)
		{
			this->mOrigin = origin;
			this->mDir = dir;

			invDir = glm::vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
			sign[0] = (invDir.x < 0);
			sign[1] = (invDir.y < 0);
			sign[2] = (invDir.z < 0);
		}

		vrlib::math::Ray operator*(const glm::mat4& matrix, const Ray& ray)
		{
			glm::vec4 transOrigin = matrix * glm::vec4(ray.mOrigin, 1);
			glm::vec4 transDir = matrix * glm::vec4(ray.mDir, 0);
			return Ray(glm::vec3(transOrigin), glm::vec3(transDir));
		}

		bool Ray::planeIntersection(const Plane &plane, float &t) const
		{
			float Denominator = glm::dot(mDir, plane.normal);
			if (fabs(Denominator) <= 0.001f) // Parallel to the plane
			{
				return false;
			}
			float Numerator = glm::dot(mOrigin, plane.normal) + plane.f;
			t = -Numerator / Denominator;
			/*			if (t < 0.0f || t > 1.0f) // The intersection point is not on the line
			{
			return false;
			}*/
			return true;
		}

		bool Ray::LineIntersectPolygon(const glm::vec3* vertices, int count, float &t) const
		{
			Plane plane;
			plane.normal = glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
			if (glm::length(plane.normal) < 0.000001f)
				return false;
			plane.normal = glm::normalize(plane.normal);
			plane.f = -glm::dot(plane.normal, vertices[0]);
			float tt;

			if (!planeIntersection(plane, tt))
				return false;

			glm::vec3 intersection = mOrigin + mDir * tt;

			/*	if (Intersection == EndLine)
			return false;*/
			for (int vertex = 0; vertex < count; vertex++)
			{
				Plane edgePlane;
				int NextVertex = (vertex + 1) % count;
				glm::vec3 EdgeVector = vertices[NextVertex] - vertices[vertex];
				edgePlane.normal = glm::normalize(glm::cross(EdgeVector, plane.normal));
				edgePlane.f = -glm::dot(edgePlane.normal, vertices[vertex]);

				if (glm::dot(edgePlane.normal, intersection) + edgePlane.f > 0.0f)
					return false;
			}

			t = tt;
			return true;
		}

	}
}
