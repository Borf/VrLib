#include "Node.h"
#include "Component.h"
#include "Scene.h"
#include <assert.h>
#include <algorithm>

#ifdef WIN32
#include <rpc.h>
#endif

#include <VrLib/json.h>

#include "components/Transform.h"
#include "components/RigidBody.h"
#include "components/Renderable.h"
#include "components/Collider.h"
#include "components/Light.h"

namespace vrlib
{
	namespace tien
	{
		Node::Node(const std::string &name, Node* parent) :
			name(name)
		{
			this->transform = nullptr;
			this->rigidBody = nullptr;
			this->renderAble = nullptr;
			this->light = nullptr;
			this->parent = parent;

#ifdef WIN32
			UUID uuid = { 0 };
			::UuidCreate(&uuid);
			unsigned char* cuuid = nullptr;
			::UuidToString(&uuid, &cuuid);
			guid = std::string((char*)cuuid);
			::RpcStringFree(&cuuid);
#endif
			if (parent)
			{
				parent->setTreeDirty(this, true);
				parent->children.push_back(this);
			}
		}
				
		Node::~Node()
		{
			if (parent)
			{
				parent->children.erase(std::find(parent->children.begin(), parent->children.end(), this));
				parent->setTreeDirty(this, false);
				for (auto c : components)
					delete c;
			}
			for (auto c : children)
				delete c;
		}

		json::Value Node::asJson() const
		{
			vrlib::json::Value v;
			v["name"] = name;
			v["uuid"] = guid;
			for (auto c : components)
				v["components"].push_back(c->toJson());
			for (auto c : children)
				v["children"].push_back(c->asJson());
			return v;
		}


		Scene &Node::getScene()
		{
			return parent->getScene();
		}

		void Node::fortree(const std::function<void(Node*)> &callback)
		{
			callback(this);
			for (auto c : children)
				c->fortree(callback);
		}

		void Node::addComponent(Component* component)
		{
			assert(!component->node);
			components.push_back(component);
			component->node = this;

			if (parent)
			{
				Scene& scene = getScene();
				if (!transform)
					transform = dynamic_cast<components::Transform*>(component);
				if (!light)
					light = dynamic_cast<components::Light*>(component);
				if (!rigidBody)
				{
					rigidBody = dynamic_cast<components::RigidBody*>(component);
					if (rigidBody)
						getScene().addRigidBody(this);
				}
				if (dynamic_cast<components::Collider*>(component))
				{
					if (rigidBody)
						rigidBody->updateCollider(getScene().world);
				}

				if (!renderAble)
					renderAble = dynamic_cast<components::Renderable*>(component);
			}

		}
		template<> components::Light* Node::getComponent<components::Light>() { return light; }
		template<> components::Transform* Node::getComponent<components::Transform>() { return transform; }
		template<> components::RigidBody* Node::getComponent<components::RigidBody>() { return rigidBody; }

	}
}