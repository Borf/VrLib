#include "Polygon.h"
#include <limits>

#include <externals/poly2tri/poly2tri.h>

namespace vrlib
{
	namespace math
	{

		glm::vec2 Polygon2::normal(int index) const
		{
			int index2 = (index + 1) % size();
			return glm::normalize(glm::vec2(at(index2).y - at(index).y, -(at(index2).x - at(index).x)));
		}

		Polygon2::Polygon2(p2t::Triangle *t)
		{
			for (int i = 0; i < 3; i++)
				push_back(glm::vec2(t->GetPoint(i)->x, t->GetPoint(i)->y));
		}

		Polygon2::Polygon2(const std::vector<glm::vec2> &l) : std::vector<glm::vec2>(l)
		{

		}

		Polygon2::Polygon2(const std::initializer_list<glm::vec2> &s) : std::vector<glm::vec2>(s)
		{

		}


		std::vector<p2t::Point*> Polygon2::toP2TPolygon() const
		{
			std::vector<p2t::Point*> ret;
			for (size_t i = 0; i < size(); i++)
				ret.push_back(new p2t::Point(at(i)));
			return ret;
		}

		//TODO: unsure if this works for concave polygons properly. also check http://alienryderflex.com/polygon/
		bool Polygon2::contains(glm::vec2 point) const
		{
			//http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
			int i, j, c = 0;
			for (i = 0, j = (int)size() - 1; i < (int)size(); j = i++) {
				if ((((*this)[i].y>point.y) != ((*this)[j].y > point.y)) &&
					(point.x < ((*this)[j].x - (*this)[i].x) * (point.y - (*this)[i].y) / (float)((*this)[j].y - (*this)[i].y) + (*this)[i].x))
					c = !c;
			}
			return c != 0;
		}

		glm::vec2 Polygon2::projectClosest(const glm::vec2 &position) const
		{
/*			float minDist = 9999;
			glm::vec2 closestPoint;

			for (size_t i = 0; i < size(); i++)
			{
				blib::math::Line line(at(i), at((i + 1) % size()));
				glm::vec2 p = line.project(position);
				float dist = glm::distance(p, position);
				if (dist < minDist)
				{
					minDist = dist;
					closestPoint = p;
				}
			}
			return closestPoint;*/
			return at(0);
		}

		bool Polygon2::isConvex() const
		{
			bool result;
			for (size_t i = 0; i < size(); i++)
			{
				int j = (i + 1) % size();
				int k = (i + 2) % size();

				float z = (at(j).x - at(i).x) * (at(k).y - at(j).y);
				z -= (at(j).y - at(i).y) * (at(k).x - at(j).x);;

				if (i == 0)
					result = std::signbit(z);
				if (std::signbit(z) != result)
					return false;
			}
			return true;
		}

		bool Polygon2::clockwise() const
		{
			int i = 0;
			int j = (i + 1) % size();
			int k = (i + 2) % size();

			float z = (at(j).x - at(i).x) * (at(k).y - at(j).y);
			z -= (at(j).y - at(i).y) * (at(k).x - at(j).x);;
			return std::signbit(z);
		}




		glm::vec2 Polygon2::getCenter() const
		{
			glm::vec2 center;
			for (const glm::vec2 &v : *this)
				center += v;
			center /= (float)size();
			return center;
		}

	}
}