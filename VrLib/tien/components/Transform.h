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
				friend class vrlib::tien::Node;
			public:
				glm::vec3 position;
				glm::quat rotation;
				glm::vec3 scale;

				glm::mat4 transform;
				glm::mat4 globalTransform;

				Transform(const glm::vec3 &position = glm::vec3(0, 0, 0), const glm::quat &rotation = glm::quat(), const glm::vec3 &scale = glm::vec3(1, 1, 1));
				Transform(const vrlib::json::Value &json);
				~Transform();
				json::Value toJson(json::Value &meshes) const override;


				void lookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up);
				void lookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0,1,0));
				void setGlobalPosition(const glm::vec3 &position, bool resetPhyics = true);
				void setGlobalRotation(const glm::quat &rotation);
				void setGlobalScale(const glm::vec3 &scale);

				glm::vec3 getGlobalPosition() const;
				glm::quat getGlobalRotation() const;
				glm::vec3 getGlobalScale() const;

				bool moveTo(const glm::vec3 &target, float speed);
				void rotate(const glm::vec3 &angle);

				virtual void buildEditor(EditorBuilder* builder);

			};
		}
	}
}