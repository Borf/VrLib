#include "Node.h"
#include "Component.h"
#include "Scene.h"
#include <assert.h>
#include <algorithm>

#include "components/Transform.h"
#include "components/RigidBody.h"
#include "components/Renderable.h"
#include "components/Collider.h"

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
			this->parent = parent;
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

			Scene& scene = getScene();


			if(!transform)
				transform = dynamic_cast<components::Transform*>(component);
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
}