#pragma once

#include <list>
#include <vector>
#include <functional>

namespace vrlib
{
	namespace json { class Value; }
	namespace tien
	{
		class Component;
		class Scene;
		namespace components
		{
			class Transform;
			class RigidBody;
			class Renderable;
			class Light;
		}

		class Node
		{
		protected:
			virtual void setTreeDirty(Node* newNode, bool isNewNode) { if(parent) parent->setTreeDirty(newNode, isNewNode); };
			std::vector<Component*> components;
			friend class Scene;
			std::list<Node*> children;
		public:
			components::Transform* transform;
			components::RigidBody* rigidBody;
			components::Renderable* renderAble;
			components::Light* light;

			std::string guid;
			std::string name;
			Node* parent;

			Node(const std::string &name, Node* parent);
			Node(const Node& other) = delete;
			~Node();

			Node* getFirstChild() { return children.empty() ? nullptr : children.front(); }

			json::Value asJson() const;

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


			template<class T> std::vector<T*> getComponents()
			{
				std::vector<T*> ret;
				for (auto c : components)
				{
					T* r = dynamic_cast<T*>(c);
					if (r)
						ret.push_back(r);
				}
				return ret;
			}
			std::vector<Component*> getComponents()
			{
				return components;
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
			Node* findNodeWithGuid(const std::string &guid)
			{
				if (this->guid == guid) //TODO: add better compare here
					return this;
				for (auto c : children)
				{
					Node* cn = c->findNodeWithGuid(guid);
					if (cn)
						return cn;
				}
				return nullptr;
			}


			virtual Scene& getScene();
			void fortree(const std::function<void(Node*)> &callback);
			void addComponent(Component* component);
		};
		template<> components::Light* Node::getComponent<components::Light>();
		template<> components::Transform* Node::getComponent<components::Transform>();
		template<> components::RigidBody* Node::getComponent<components::RigidBody>();

	}
}