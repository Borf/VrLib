#include "Node.h"
#include "Component.h"
#include "Scene.h"
#include <assert.h>
#include <algorithm>

#ifdef WIN32
#include <rpc.h>
#endif

#include <VrLib/json.hpp>
#include <VrLib/util.h>

#include "components/Transform.h"
#include "components/RigidBody.h"
#include "components/Renderable.h"
#include "components/Collider.h"
#include "components/Light.h"
#include "components/Camera.h"
#include "components/AnimatedModelRenderer.h"
#include "components/ModelRenderer.h"
#include "components/DynamicSkyBox.h"
#include "components/MeshRenderer.h"
#include "components/RigidBody.h"
#include "components/BoxCollider.h"
#include "components/MeshCollider.h"
#include "components/postprocess/Bloom.h"
#include "components/postprocess/Gamma.h"
#include "components/postprocess/DoF.h"

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
			guid = vrlib::util::getGuid();
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
			}
			for (auto c : components)
				delete c;
			while(!children.empty())
				delete children.back();
		}

		void Node::setParent(Node* newParent)
		{
			assert(parent);
			if (newParent == this)
				return;
			parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end()); //removes this from parent
			parent = newParent;
			newParent->children.push_back(this);
		}

		bool Node::isChildOf(Node * parent)
		{
			if (this == parent)
				return true;
			if (this->parent == nullptr)
				return false;
			return this->parent->isChildOf(parent);
		}

		json Node::asJson(json &meshes) const
		{
			json v;
			v["name"] = name;
			v["uuid"] = guid;
			for (auto c : components)
				v["components"].push_back(c->toJson(meshes));
			for (auto c : children)
				v["children"].push_back(c->asJson(meshes));
			return v;
		}

		void Node::fromJson(const json &data, const json &totalJson, const std::function<Component*(const json &)> &callback)
		{
			setTreeDirty(this, true);
			name = data["name"].get<std::string>();
			guid = data["uuid"].get<std::string>();

			for (auto c : components)
				delete c;
			while(!children.empty())
				delete children.front();
			components.clear();
			transform = nullptr;
			rigidBody = nullptr;
			renderAble = nullptr;
			light = nullptr;


			if(data.find("components") != data.end())
				for (auto c : data["components"])
				{
					if (c.find("type") == c.end())
						continue;
					if (c["type"] == "transform")
					{
						addComponent(new vrlib::tien::components::Transform(c));
						transform->buildTransform((parent && parent->transform) ? parent->transform->globalTransform : glm::mat4());
					}
					else if (c["type"] == "camera")
						addComponent(new vrlib::tien::components::Camera(c));
					else if (c["type"] == "modelrenderer")
						addComponent(new vrlib::tien::components::ModelRenderer(c));
					else if (c["type"] == "animatedmodelrenderer")
						addComponent(new vrlib::tien::components::AnimatedModelRenderer(c));
					else if (c["type"] == "dynamicskybox")
						addComponent(new vrlib::tien::components::DynamicSkyBox(c));
					else if (c["type"] == "light")
						addComponent(new vrlib::tien::components::Light(c));
					else if (c["type"] == "meshrenderer")
						addComponent(new vrlib::tien::components::MeshRenderer(c, totalJson));
					else if (c["type"] == "rigidbody")
						addComponent(new vrlib::tien::components::RigidBody(c));
					else if (c["type"] == "collider")
					{
						if(c["collider"] == "box")
							addComponent(vrlib::tien::components::BoxCollider::fromJson(c, &getScene()));
						if (c["collider"] == "mesh")
						{
							bool convex = true;
							if (c.find("convex") != c.end())
								convex = c["convex"];
							addComponent(new vrlib::tien::components::MeshCollider(this, convex));
						}
					}
					else if (c["type"] == "postprocess")
					{
						if (c["postprocess"] == "bloom")
							addComponent(new vrlib::tien::components::postprocessors::Bloom());
						if (c["postprocess"] == "gamma")
							addComponent(new vrlib::tien::components::postprocessors::Gamma());
						if (c["postprocess"] == "dof")
							addComponent(new vrlib::tien::components::postprocessors::DoF());
					}
					else
					{
						if (callback)
						{
							vrlib::tien::Component* newComponent = callback(c);
							if (newComponent)
								addComponent(newComponent);
							else
								logger << "Unhandled component: " << c["type"] << Log::newline;
						}
					}
				}

			if(data.find("children") != data.end())
				for (auto c : data["children"])
					(new Node("", this))->fromJson(c, totalJson, callback);
		}

		void Node::updateNodePointer(vrlib::tien::Node * oldNode, vrlib::tien::Node * newNode)
		{
			for (size_t i = 0; i < children.size(); i++)
			{
				if (children[i] == oldNode)
					children[i] = newNode;
			}
			for (auto c : components)
				c->updateNodePointer(oldNode, newNode);
			for (auto c : children)
				c->updateNodePointer(oldNode, newNode);
		}

		void Node::addDebugChildSphere()
		{
			auto n = new vrlib::tien::Node("debug", this);
			n->addComponent(new vrlib::tien::components::Transform(glm::vec3(0, 0, 0), glm::quat(), glm::vec3(0.02f, 0.02f, 0.02f)));
			n->addComponent(new vrlib::tien::components::ModelRenderer("sphere.shape"));
		}



		Node * Node::findNodeWithName(const std::string & name)
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

		std::vector<Node*> Node::findNodesWithName(const std::string & name)
		{
			std::vector<Node*> ret;
			fortree([&ret, &name](Node* n) {
				if (n->name == name)
					ret.push_back(n);
			});
			return ret;
		}

		Node * Node::findNodeWithGuid(const std::string & guid)
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

		void Node::sortChildren()
		{
			std::sort(children.begin(), children.end(), [](Node* a, Node* b)
			{
				if (a->children.empty() != b->children.empty())
					return !a->children.empty();
				
				if (a->getComponent<components::ModelRenderer>() != b->getComponent<components::ModelRenderer>())
					return a->getComponent<components::ModelRenderer>() == nullptr;

				return a->name < b->name;
			});
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

		void Node::fortree(const std::function<void(const Node*)> &callback) const
		{
			callback(this);
			for (const Node* c : children)
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
				{
					transform = dynamic_cast<components::Transform*>(component);
					if (transform)
					{
						if (parent && parent->transform) //todo: what if parent has no transform, but parent-parent does
							transform->globalTransform = parent->transform->globalTransform * transform->transform;
						else
							transform->globalTransform = transform->transform; //tmp
					}
				}
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
					{
						/*if (rigidBody->body->getCollisionShape() == components::RigidBody::emptyShape)
						{ //if the body doesn't have a collisionshape, the worldtransform will change due to the offset calculation in the getTransform in the rigidbody
							btTransform wt = rigidBody->body->getWorldTransform();
							glm::vec3 offset = getComponent<components::Collider>()->offset; //TODO: null check
							wt.setOrigin(wt.getOrigin() + btVector3(offset.x, offset.y, offset.z));
							rigidBody->body->setWorldTransform(wt);
						}
						rigidBody->updateCollider(getScene().world);*/
						rigidBody->updateCollider(getScene().gScene);
					}
				}

				if (!renderAble)
					renderAble = dynamic_cast<components::Renderable*>(component);
			}

		}
		template<> components::Light* Node::getComponent<components::Light>()  const { return light; }
		template<> components::Transform* Node::getComponent<components::Transform>()  const { return transform; }
		template<> components::RigidBody* Node::getComponent<components::RigidBody>()  const { return rigidBody; }

	}
}