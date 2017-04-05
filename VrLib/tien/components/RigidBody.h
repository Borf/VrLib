#pragma once

#include "../Component.h"

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

namespace vrlib
{
	namespace tien
	{
		class Scene;
		class Node;

		namespace components
		{
			class RigidBody : public Component
			{
			public:
				enum class Type
				{
					Static,
					Kinematic,
					Dynamic,
					Auto
				};
			private:
				friend class vrlib::tien::Scene;
				friend class vrlib::tien::Node;
				
				float mass;
				void init(physx::PxScene* scene);
				void updateCollider(physx::PxScene* scene);

				Type type;

			public:
				physx::PxScene* world;

				RigidBody(float mass, Type type = Type::Auto);
				RigidBody(const json &json);
				~RigidBody();
				physx::PxRigidActor* actor;
				json toJson(json &meshes) const override;
				void buildEditor(EditorBuilder * builder, bool folded) override;
				inline float getMass() const {return mass; };
				void setMass(float newMass);
				virtual void postUpdate(Scene& scene);

				inline Type getType() const { return type; };
				void setType(Type newType);


				void resetMotion();
				//void setLinearFactor(const glm::vec3 &axis) { actor->setLinearFactor(btVector3(axis.x, axis.y, axis.z)); }
				//void setAngularFactor(const glm::vec3 axis) { actor->setAngularFactor(btVector3(axis.x, axis.y, axis.z)); };



			};
		}
	}
}