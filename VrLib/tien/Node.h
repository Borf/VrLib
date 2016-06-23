#pragma once

#include <list>
#include <vector>
#include <functional>

namespace vrlib
{
	namespace tien
	{
		class Component;

		class Node
		{
		protected:
			virtual void setTreeDirty(Node* newNode, bool isNewNode) { if(parent) parent->setTreeDirty(newNode, isNewNode); };
			std::vector<Component*> components;
			friend class Scene;

			Node(const Node* original);

			Node &operator =(const Node &other);

		public:
			std::string name;
			Node* parent;
			const Node* orig;
			std::list<Node*> children;


			template<class T> T* getComponent()
			{
				for (auto c : components)
				{
					T* r = dynamic_cast<T*>(c);
					if (r) 
						return r;
				}
				return nullptr;
			}


			template<class T> Node* findNodeWithComponent()
			{
				if (getComponent<T>())
					return this;
				for (auto c : children)
				{
					Node* cn = c->findNodeWithComponent<T>();
					if (cn)
						return cn;
				}
				return nullptr;
			}

			Node* findNodeWithName(const std::string &name)
			{
				if (this->name == name)
					return this;
				for (auto c : children)
				{
					Node* cn = c->findNodeWithName(name);
					if (cn)
						return cn;
				}
				return nullptr;
			}


			Node(const std::string &name, Node* parent);
			~Node();

			void fortree(const std::function<void(Node*)> &callback);
			void addComponent(Component* component);
		};

	}
}