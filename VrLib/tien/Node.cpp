#include "Node.h"
#include "Component.h"
#include <assert.h>
#include <algorithm>

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
				parent->setTreeDirty(this, true);
				parent->children.push_back(this);
			}
		}

		Node::Node(const Node* original)
		{
			orig = original;
			name = original->name;
			parent = nullptr;

			components = original->components;

			for (auto c : original->children)
			{
				Node* newChild = new Node(c);
				children.push_back(newChild);
				newChild->parent = this;
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