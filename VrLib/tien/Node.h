#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <type_traits>

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
			friend class components::Renderable;
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
			bool isChildOf(Node* parent);

			json::Value asJson(json::Value &meshes) const;
			void fromJson(const json::Value &json, const json::Value &totalJson, const std::function<Component*(const json::Value &)> & = nullptr);

			void addDebugChildSphere();

			template<class T> T* getComponent() const
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
				components.erase(std::remove_if(components.begin(), components.end(), [this](Component* c) { 
					T* t = dynamic_cast<T*>(c);
					if (t)
						delete t;
					if (std::is_same<T, components::Renderable>::value)
						renderAble = nullptr;
					if (std::is_same<T, components::Light>::value)
						light = nullptr;
					//TODO: etc

					return t != nullptr; 
				}), components.end());
			}

			template<class T> void removeComponent(T* c)
			{
				components.erase(std::remove(components.begin(), components.end(), c), components.end());
				if (std::is_same<T, components::Renderable>::value)
					renderAble = nullptr;
				if (std::is_same<T, components::Light>::value)
					light = nullptr;
				//TODO: etc
				delete c;
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


			template<class T> const Node* findNodeWithComponent() const
			{
				if (getComponent<T>())
					return this;
				for (auto c : children)
				{
					const Node* cn = c->findNodeWithComponent<T>();
					if (cn)
						return cn;
				}
				return nullptr;
			}

			template<class T> std::vector<Node*> findNodesWithComponent()
			{
				std::vector<vrlib::tien::Node*> ret;

				if (getComponent<T>())
					ret.push_back(this);
				for (auto c : children)
				{
					auto v2 = c->findNodesWithComponent<T>();
					ret.insert(ret.end(), v2.begin(), v2.end());
				}
				return ret;
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

			void sortChildren();

			virtual Scene& getScene();
			void fortree(const std::function<void(Node*)> &callback);
			void fortree(const std::function<void(const Node*)> &callback) const;
			void addComponent(Component* component);
		};
		template<> components::Light* Node::getComponent<components::Light>() const;
		template<> components::Transform* Node::getComponent<components::Transform>() const;
		template<> components::RigidBody* Node::getComponent<components::RigidBody>() const;

	}
}