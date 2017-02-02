#include "Transform.h"
#include <VrLib/json.hpp>
#include <VrLib/tien/Node.h>
#include <VrLib/tien/components/RigidBody.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

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
				buildTransform();
			}

			Transform::Transform(const json & json)
			{
				position = glm::vec3(json["position"][0], json["position"][1], json["position"][2]);
				rotation = glm::quat(json["rotation"][3], json["rotation"][0], json["rotation"][1], json["rotation"][2]);
				scale = glm::vec3(json["scale"][0], json["scale"][1], json["scale"][2]);
				buildTransform();
			}

			Transform::~Transform()
			{

			}

			json Transform::toJson(json &meshes) const
			{
				json ret;
				ret["type"] = "transform";
				for (int i = 0; i < 3; i++)
					ret["position"].push_back(position[i]);
				for (int i = 0; i < 4; i++)
					ret["rotation"].push_back(rotation[i]);
				for (int i = 0; i < 3; i++)
					ret["scale"].push_back(scale[i]);
				return ret;
			}

			void Transform::setGlobalPosition(const glm::vec3 &position, bool resetPhyics)
			{
				glm::mat4 parentMat;
				if (node->parent && node->parent->transform)
					parentMat = node->parent->transform->globalTransform;

				parentMat = glm::inverse(parentMat);
				this->position = glm::vec3(parentMat * glm::vec4(position, 1));

				auto rigidBody = node->getComponent<RigidBody>();
				if (resetPhyics && rigidBody && rigidBody->body)
				{
					btTransform t;
					rigidBody->body->getMotionState()->getWorldTransform(t);
					rigidBody->body->setWorldTransform(t);
					rigidBody->body->setLinearVelocity(btVector3(0, 0, 0));
				}
			}


			glm::quat Transform::getGlobalRotation() const
			{
				return glm::quat(globalTransform);

				std::function<glm::quat(Node*)> parentRot;
				parentRot = [&parentRot](Node* n)
				{
					glm::quat rot;
					if (n->transform)
						rot = n->transform->rotation;
					if (n->parent)
						return parentRot(n->parent) * rot;
					return rot;
				};
				return parentRot(node);
			}

			glm::vec3 Transform::getGlobalScale() const
			{
				std::function<glm::vec3(Node*)> parentScale;
				parentScale = [&parentScale](Node* n)
				{
					glm::vec3 scale(1,1,1);
					if (n->transform)
						scale = n->transform->scale;
					if (n->parent)
						return parentScale(n->parent) * scale;
					return scale;
				};
				return parentScale(node);

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

			glm::vec3 Transform::getGlobalPosition() const
			{
				return glm::vec3(globalTransform * glm::vec4(0,0,0,1));
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




			void Transform::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Transform");

				if (folded)
					return;

				builder->beginGroup("Local Translate", false);
				builder->addTextBox(builder->toString(position.x), [this](const std::string & newValue) { position.x = (float)atof(newValue.c_str());  });
				builder->addTextBox(builder->toString(position.y), [this](const std::string & newValue) { position.y = (float)atof(newValue.c_str());  });
				builder->addTextBox(builder->toString(position.z), [this](const std::string & newValue) { position.z = (float)atof(newValue.c_str());  });
				builder->endGroup();

				builder->beginGroup("Local Scale", false);
				builder->addTextBox(builder->toString(scale.x), [this](const std::string & newValue) { scale.x = (float)atof(newValue.c_str());  });
				builder->addTextBox(builder->toString(scale.y), [this](const std::string & newValue) { scale.y = (float)atof(newValue.c_str());  });
				builder->addTextBox(builder->toString(scale.z), [this](const std::string & newValue) { scale.z = (float)atof(newValue.c_str());  });
				builder->addCheckbox(true, [](bool newValue) {});
				builder->endGroup();

				glm::vec3 euler = glm::eulerAngles(rotation);

				//TODO: use yaw/pitch/roll for rotation
				builder->beginGroup("Local Rotation", false);
				builder->addTextBox(builder->toString(glm::degrees(euler.x)), [this](const std::string & newValue) {
					glm::vec3 euler = glm::eulerAngles(rotation);
					euler.x = (float)glm::radians(atof(newValue.c_str()));
					rotation = glm::quat(euler);
				});
				builder->addTextBox(builder->toString(glm::degrees(euler.y)), [this](const std::string & newValue) {
					glm::vec3 euler = glm::eulerAngles(rotation);
					euler.y = (float)glm::radians(atof(newValue.c_str()));
					rotation = glm::quat(euler);
				});
				builder->addTextBox(builder->toString(glm::degrees(euler.z)), [this](const std::string & newValue) {
					glm::vec3 euler = glm::eulerAngles(rotation);
					euler.z = (float)glm::radians(atof(newValue.c_str()));
					rotation = glm::quat(euler);
				});
				builder->endGroup();


				glm::vec3 globalPosition = getGlobalPosition();

				builder->beginGroup("Global Translate", false);
				for(int i = 0; i < 3; i++)
				builder->addTextBox(builder->toString(globalPosition[i]), [this,i](const std::string & newValue) { auto gp = getGlobalPosition(); gp[i] = (float)atof(newValue.c_str()); setGlobalPosition(gp);  });
				builder->endGroup();


			}
		}
	}
}