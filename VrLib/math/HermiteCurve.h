#pragma once

#include <glm/glm.hpp>


namespace vrlib
{
	namespace math
	{
		template<class T = glm::vec2>
		class HermiteCurve
		{
		public:
			T p0;
			T p1;

			T m0;
			T m1;


			HermiteCurve(const T &pos0, const T &dir0, const T &pos1, const T &dir1);
			
			T getPoint(float t) const;
			float getLength() const;

		};
	}
}
