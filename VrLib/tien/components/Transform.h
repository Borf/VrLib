#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <VrLib/json.hpp>
#include <VrLib/DirtyFlag.h>
#include "../Component.h"

namespace vrlib
{
	namespace tien
	{
		class Scene;

		namespace components
		{
			class TransformAttach;

			class Transform : public Component
			{
			private:
				void buildTransform(const glm::mat4 &parentTransform);
				friend class vrlib::tien::Scene;
				friend class vrlib::tien::Node;
				friend class vrlib::tien::components::TransformAttach;
				bool staticTransform = false;
			public:
				DirtyFlag<glm::vec3> position;
				DirtyFlag<glm::quat> rotation;
				DirtyFlag<glm::vec3> scale;

				glm::mat4 transform;
				glm::mat4 globalTransform;


				Transform(const glm::vec3 &position = glm::vec3(0, 0, 0), const glm::quat &rotation = glm::quat(), const glm::vec3 &scale = glm::vec3(1, 1, 1));
				Transform(const json &data);
				~Transform();
				json toJson(json &meshes) const override;


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

				void setStatic(bool newValue);
				bool isStatic() { return staticTransform; }

				virtual void buildEditor(EditorBuilder* builder, bool folded) override;

			};
		}
	}
}