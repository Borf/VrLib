#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Component.h"

namespace vrlib
{
	namespace tien
	{
		class Scene;

		namespace components
		{
			class Transform : public Component
			{
			private:
				void buildTransform();
				friend class vrlib::tien::Scene;
			public:
				glm::vec3 position;
				glm::quat rotation;
				glm::vec3 scale;

				glm::mat4 transform;
				glm::mat4 globalTransform;

				Transform(const glm::vec3 &position = glm::vec3(0, 0, 0), const glm::quat &rotation = glm::quat(), const glm::vec3 &scale = glm::vec3(1, 1, 1));
				~Transform();
				json::Value toJson() const override;


				void lookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);
				void lookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0,1,0));
				void setGlobalPosition(const glm::vec3 &position);
				void setGlobalRotation(const glm::quat &rotation);
				void setGlobalScale(const glm::vec3 &scale);


				bool moveTo(const glm::vec3 &target, float speed);
				void rotate(const glm::vec3 &angle);


			};
		}
	}
}