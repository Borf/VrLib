#pragma once

#include <stdio.h>

template <class T>
class DirtyFlag : public T
{
private:
	T copy;
public:
	DirtyFlag()
	{
		//printf("Sizeof: %i - %i\n", sizeof(*this), sizeof(T));
		//TODO: make it dirty
	}
	DirtyFlag(const T& value) : T(value)
	{//TODO: make it dirty
	}

	virtual bool isDirty()
	{
		return copy != *dynamic_cast<T*>(this);
	}
	virtual void markClean()
	{
		copy = *dynamic_cast<T*>(this);
	}
};

#include <glm/gtc/type_ptr.hpp>

namespace glm
{
	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T const * value_ptr
	(
		DirtyFlag<glm::tvec3<T, P> > const & vec
	)
	{
		return &(vec.x);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T const * value_ptr
	(
		DirtyFlag<tmat4x4<T, P> > const & mat
	)
	{
		return &(mat[0].x);
	}

}