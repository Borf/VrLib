#include "Node.h"
#include "Component.h"
#include <assert.h>

namespace vrlib
{
	namespace tien
	{
		Node::Node(const std::string &name, Node* parent) :
			name(name)
		{
			this->orig = nullptr;
			this->parent = parent;
			if (parent)
			{
				parent->setTreeDirty();
				parent->children.push_back(this);
			}
		}

		Node::Node(const Node* original)
		{
			orig = original;
			name = original->name;

			components = original->components;

			for (auto c : original->children)
			{
				Node* newChild = new Node(c);
				children.push_back(newChild);
				newChild->parent = this;
			}
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
		}

	}
}