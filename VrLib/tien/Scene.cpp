#include "Scene.h"
#include "components/ModelRenderer.h"
#include "components/Light.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/RigidBody.h"
#include "components/Collider.h"

#include <btBulletDynamicsCommon.h>


namespace vrlib
{
	namespace tien
	{
		Scene::Scene() : Node("Root", nullptr)
		{
			cameraNode = nullptr;
			world = nullptr;
		}

		Scene::Scene(const Scene& other) : Node(&other)
		{
			cameraNode = other.cameraNode;
			treeDirty = true;
			world = nullptr;
			update(0);
		}


		void Scene::setTreeDirty()
		{
			treeDirty = true;
		}


		void Scene::updateRenderables()
		{
			renderables.clear();
			lights.clear();
			fortree([this](Node* n)
			{
				if (n->getComponent<components::ModelRenderer>())
					renderables.push_back(n);
				if (n->getComponent<components::Light>())
					lights.push_back(n);
			});
		}

		void Scene::prepareForRun()
		{
			broadphase = new btDbvtBroadphase();
			collisionConfiguration = new btDefaultCollisionConfiguration();
			dispatcher = new btCollisionDispatcher(collisionConfiguration);
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
			world->setGravity(btVector3(0, -9.8f, 0));



			fortree([this](Node* n)
			{
				components::RigidBody* rigidBody;
				if (rigidBody = n->getComponent<components::RigidBody>())
					rigidBody->init(world);
			});
		}



		void Scene::update(float elapsedTime)
		{
			if (treeDirty)
			{
				updateRenderables();
				if (!cameraNode)
					cameraNode = findNodeWithComponent<components::Camera>();
				treeDirty = false;
			}
			if(world)
				world->stepSimulation(elapsedTime);
			fortree([this, &elapsedTime](Node* n)
			{
				for (Component* c : n->components)
					c->update(elapsedTime, *this);
			});

			//TODO: update transform matrices

			std::function<void(Node*, const glm::mat4 &)> updateTransforms;
			updateTransforms = [this, &updateTransforms](Node* n, const glm::mat4 &parentTransform)
			{
				components::Transform* transform = n->getComponent<components::Transform>();
				if (transform)
				{
					transform->buildTransform();
					transform->globalTransform = parentTransform * transform->transform;
					for (auto c : n->children)
						updateTransforms(c, transform->globalTransform);
				}
				else
					for (auto c : n->children)
						updateTransforms(c, parentTransform);
			};
			updateTransforms(this, glm::mat4());
		}


	}
}