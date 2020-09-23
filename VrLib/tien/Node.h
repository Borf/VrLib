#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <list>
#include <type_traits>
#include <VrLib/json.hpp>

#include "components/Renderable.h"
#include "components/Light.h"
#include "components/RigidBody.h"
#include "components/Transform.h"

namespace vrlib
{
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
			std::list<components::Renderable*> renderAbles;
			components::Light* light;
			bool enabled = true;
			std::string guid;
			std::string name;
			std::string prefabFile;
			Node* parent;

			Node(const std::string &name, Node* parent);
			Node(const Node& other) = delete;
			~Node();

			Node* getFirstChild() const { return children.empty() ? nullptr : children.front(); }
			Node* getNextSibling() const;
			void setParent(Node* newParent);
			bool isChildOf(Node* parent) const;

			json asJson(json &meshes) const;
			void fromJson(const json &data, const json &totalJson, const std::function<Component*(const json &)> & = nullptr);
			void fromJson(const json &data, const json &totalJson, const std::function<Component*(const json &, const std::string)> & = nullptr);
			
			void updateNodePointer(vrlib::tien::Node* oldNode, vrlib::tien::Node* newNode);

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

					auto r = dynamic_cast<components::Renderable*>(t);
					if (r)
					{
						renderAbles.remove(r);
						setTreeDirty(this, false);
					}
					if (std::is_same<T, components::Light>::value)
						light = nullptr;
					if (std::is_same<T, components::RigidBody>::value)
						rigidBody = nullptr;
					//TODO: etc

					if (t)
						delete t;
					return t != nullptr; 
				}), components.end());
			}

			template<class T> void removeComponent(T* c)
			{
				components.erase(std::remove(components.begin(), components.end(), c), components.end());
				auto r = dynamic_cast<components::Renderable*>(c);
				if (r)
				{
					renderAbles.remove(r);
					setTreeDirty(this, false);
				}
				if (std::is_same<T, components::Light>::value)
					light = nullptr;
				if (std::is_same<T, components::RigidBody>::value)
					rigidBody = nullptr;
				//TODO: etc
				delete c;
			}


			template<class T> std::vector<T*> getComponents() const
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
			std::vector<Component*> getComponents() const
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