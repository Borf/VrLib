#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			Transform::Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale) : 
				position(position), 
				rotation(rotation), 
				scale(scale)
			{

			}

			Transform::~Transform()
			{

			}

			void Transform::setGlobalPosition(const glm::vec3 &position)
			{
				this->position = position;//TODO
			}

			void Transform::setGlobalRotation(const glm::quat &rotation)
			{
				this->rotation = rotation;//TODO
			}

			void Transform::buildTransform()
			{
				transform = glm::mat4();
				transform = glm::translate(transform, position);
				transform = transform * glm::toMat4(rotation);
				transform = glm::scale(transform, scale);
			}


			void Transform::lookAt(const glm::vec3 &target, const glm::vec3 &up)
			{
				glm::mat4 mat = glm::lookAt(position, target, up);
				rotation = glm::inverse(glm::quat(mat));
			}

			bool Transform::moveTo(const glm::vec3 &target, float speed)
			{
				glm::vec3 diff = target - position;
				float len = glm::length(diff);
				if (len < speed)
				{
					position = target;
					return true;
				}
				else
				{
					diff /= len;
					diff *= speed;
					position += diff;
					return false;
				}
			}

			void Transform::rotate(const glm::vec3 &angles)
			{
				rotation *= glm::quat(angles);
			}
		}
	}
}