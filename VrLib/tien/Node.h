#pragma once

#include <vector>
#include <functional>
#include <algorithm>

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
			std::vector<Node*> children;
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

			Node* getFirstChild() const { return children.empty() ? nullptr : children.front(); }
			Node* getNextSibling() const;
			void setParent(Node* newParent);

			json::Value asJson(json::Value &meshes) const;
			void fromJson(const json::Value &json, const json::Value &totalJson);

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

			template<class T> void removeComponent()
			{
				components.erase(std::remove_if(components.begin(), components.end(), [](const Component* c) { return dynamic_cast<const T*>(c) != nullptr; }), components.end());
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

			Node* findNodeWithName(const std::string &name);
			std::vector<Node*> findNodesWithName(const std::string &name);
			Node* findNodeWithGuid(const std::string &guid);

			inline std::vector<Node*> getChildren() const
			{
				return std::vector<Node*>(children);
			}

			inline Node* getChild(int index) const
			{
				return children[index];
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