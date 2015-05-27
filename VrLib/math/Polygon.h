#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace p2t { struct Point; class Triangle; }

namespace vrlib
{
	namespace math
	{

		class Polygon2 : public std::vector<glm::vec2>
		{
		public:
			Polygon2() { }
			Polygon2(const std::vector<glm::vec2> &l);
			Polygon2(const std::initializer_list<glm::vec2> &s);
			Polygon2(p2t::Triangle *t);


			std::vector<p2t::Point*> toP2TPolygon() const;
			bool contains(glm::vec2 point) const;
			bool isConvex() const;
			bool clockwise() const;

			glm::vec2 getCenter() const;


			glm::vec2 normal(int index) const;
			glm::vec2 projectClosest(const glm::vec2 &position) const;
		};

	}
}