#include "Transform.h"
#include <VrLib/json.h>
#include <VrLib/tien/Node.h>

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

			}

			Transform::Transform(const vrlib::json::Value & json)
			{
				position = glm::vec3(json["position"][0].asFloat(), json["position"][1].asFloat(), json["position"][2].asFloat());
				rotation = glm::quat(json["rotation"][3].asFloat(), json["rotation"][0].asFloat(), json["rotation"][1].asFloat(), json["rotation"][2].asFloat());
				scale = glm::vec3(json["scale"][0].asFloat(), json["scale"][1].asFloat(), json["scale"][2].asFloat());
			}

			Transform::~Transform()
			{

			}

			json::Value Transform::toJson(json::Value &meshes) const
			{
				json::Value ret;
				ret["type"] = "transform";
				for (int i = 0; i < 3; i++)
					ret["position"].push_back(position[i]);
				for (int i = 0; i < 4; i++)
					ret["rotation"].push_back(rotation[i]);
				for (int i = 0; i < 3; i++)
					ret["scale"].push_back(scale[i]);
				return ret;
			}

			void Transform::setGlobalPosition(const glm::vec3 &position)
			{
				glm::vec3 parentPos;
				if (node->parent && node->parent->transform)
					parentPos = node->parent->transform->getGlobalPosition();

				this->position = position - parentPos;//TODO
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


			std::string toString(float value)
			{
				std::ostringstream ss;
				ss << value;
				return ss.str();
			}


			void Transform::buildEditor(EditorBuilder * builder)
			{
				builder->addTitle("Transform");

				builder->beginGroup("Translate", false);
				builder->addTextBox(toString(position.x), [this](const std::string & newValue) { position.x = (float)atof(newValue.c_str());  });
				builder->addTextBox(toString(position.y), [this](const std::string & newValue) { position.y = (float)atof(newValue.c_str());  });
				builder->addTextBox(toString(position.z), [this](const std::string & newValue) { position.z = (float)atof(newValue.c_str());  });
				builder->endGroup();

				builder->beginGroup("Scale", false);
				builder->addTextBox(toString(scale.x), [this](const std::string & newValue) { scale.x = (float)atof(newValue.c_str());  });
				builder->addTextBox(toString(scale.y), [this](const std::string & newValue) { scale.y = (float)atof(newValue.c_str());  });
				builder->addTextBox(toString(scale.z), [this](const std::string & newValue) { scale.z = (float)atof(newValue.c_str());  });
				builder->addCheckbox(true, [](bool newValue) {});
				builder->endGroup();

				glm::vec3 euler = glm::eulerAngles(rotation);

				//TODO: use yaw/pitch/roll for rotation
				builder->beginGroup("Rotation", false);
				builder->addTextBox(toString(glm::degrees(euler.x)), [this](const std::string & newValue) {
					glm::vec3 euler = glm::eulerAngles(rotation);
					euler.x = (float)glm::radians(atof(newValue.c_str()));
					rotation = glm::quat(euler);
				});
				builder->addTextBox(toString(glm::degrees(euler.y)), [this](const std::string & newValue) {
					glm::vec3 euler = glm::eulerAngles(rotation);
					euler.y = (float)glm::radians(atof(newValue.c_str()));
					rotation = glm::quat(euler);
				});
				builder->addTextBox(toString(glm::degrees(euler.z)), [this](const std::string & newValue) {
					glm::vec3 euler = glm::eulerAngles(rotation);
					euler.z = (float)glm::radians(atof(newValue.c_str()));
					rotation = glm::quat(euler);
				});
				builder->endGroup();

			}
		}
	}
}