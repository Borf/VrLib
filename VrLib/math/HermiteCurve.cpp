#include "HermiteCurve.h"

namespace vrlib
{
	namespace math
	{
		template<class T>
		HermiteCurve<T>::HermiteCurve(const T &pos0, const T &dir0, const T &pos1, const T &dir1)
		{
			this->p0 = pos0;
			this->p1 = pos1;
			this->m0 = dir0;
			this->m1 = dir1;
		}

		template<class T>
		T HermiteCurve<T>::getPoint(float t) const
		{
			return (2 * t*t*t - 3 * t*t + 1)*p0 + (t*t*t - 2 * t*t + t)*m0 + (-2 * t*t*t + 3 * t*t)*p1 + (t*t*t - t*t)*m1;
		}

		template<class T>
		float HermiteCurve<T>::getLength() const
		{
			float length = 0;
			T lastPoint = getPoint(0);
			for (float f = 0; f < 1; f += 0.01f)
			{
				T point = getPoint(f);
				length += glm::distance(point, lastPoint);
				lastPoint = point;
			}
			return length;
		}




		template class HermiteCurve<glm::vec2>;
		template class HermiteCurve<glm::vec3>;
	}
}