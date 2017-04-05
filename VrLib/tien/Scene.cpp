#include "Scene.h"
#include "components/AnimatedModelRenderer.h"
#include "components/ModelRenderer.h"
#include "components/Light.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/RigidBody.h"
#include "components/Collider.h"
#include "components/MeshRenderer.h"

#include <VrLib/math/Ray.h>
#include <VrLib/Model.h>

#include <algorithm>


namespace vrlib
{
	namespace tien
	{

		Scene::Scene() : Node("Root", nullptr)
		{
			cameraNode = nullptr;
			frustum = new math::Frustum();
		}

		Scene::Scene(const Scene& other) : Node("", nullptr)
		{
			throw "Not allowed";
		}

		void Scene::init()
		{
			gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

			gPvd = PxCreatePvd(*gFoundation);
			physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

			gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);

			physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
			sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
			gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
			sceneDesc.cpuDispatcher = gDispatcher;
			sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
			gScene = gPhysics->createScene(sceneDesc);

			physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
			if (pvdClient)
			{
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			}

			gMaterial = gPhysics->createMaterial(0.8f, 0.8f, 0.0f);
		}


		void Scene::addRigidBody(Node* node)
		{
			node->rigidBody->init(gScene);
		}
		void Scene::addCollider(Node* node)
		{
			if (node->rigidBody)
				node->rigidBody->updateCollider(gScene);
			else
				throw "Not supported yet";
		}


		void Scene::setTreeDirty(Node* newNode, bool isNewNode)
		{
			treeDirty = true;
		}


		void Scene::updateRenderables()
		{
			renderables.clear();
			lights.clear();
			fortree([this](Node* n)
			{
				components::Renderable* renderable;
				if (renderable = n->getComponent<components::Renderable>())
				{
					renderables.push_back(n);
					if (renderable->renderContextDeferred && renderContextsDeferred.find(renderable->renderContextDeferred) == renderContextsDeferred.end())
					{
						renderable->renderContextDeferred->init(); // TODO: move this to another place?
						renderContextsDeferred.insert(renderable->renderContextDeferred);
					}
					if (renderable->renderContextShadow && renderContextsShadow.find(renderable->renderContextShadow) == renderContextsShadow.end())
					{
						renderable->renderContextShadow->init(); // TODO: move this to another place?
						renderContextsShadow.insert(renderable->renderContextShadow);
					}					
					if (renderable->renderContextForward && renderContextsForward.find(renderable->renderContextForward) == renderContextsForward.end())
					{
						renderable->renderContextForward->init(); // TODO: move this to another place?
						renderContextsForward.insert(renderable->renderContextForward);
					}
				}
				if (n->getComponent<components::Light>())
					lights.push_back(n);
			});

			renderables.sort([](Node* a, Node* b) { return (int)a->renderAble->renderContextDeferred < (int)b->renderAble->renderContextDeferred; });
		}

		void Scene::update(float elapsedTime)
		{
			if (treeDirty)
			{
				updateRenderables();
				//if (!cameraNode)
				//	cameraNode = findNodeWithComponent<components::Camera>();
				treeDirty = false;
			}

			if (gScene && elapsedTime > 0)
			{
				physicsTimer += elapsedTime;
				while (physicsTimer > physicsRate)
				{
					physicsTimer -= physicsRate;
					gScene->simulate(physicsRate);
					gScene->fetchResults(true);
				}
			}


			fortree([this, &elapsedTime](Node* n)
			{
				if (!n->enabled)
					return;
				for (Component* c : n->components)
					c->update(elapsedTime, *this);
			});

			//TODO: update transform matrices

			std::function<void(Node*, const glm::mat4 &)> updateTransforms;
			updateTransforms = [this, &updateTransforms](Node* n, const glm::mat4 &parentTransform)
			{
				if (!n->enabled)
					return;
				if (n->transform && !n->rigidBody)
				{
					n->transform->buildTransform();
					n->transform->globalTransform = parentTransform * n->transform->transform;
					for (auto c : n->children)
						updateTransforms(c, n->transform->globalTransform);
				}
				else
					for (auto c : n->children)
						updateTransforms(c, parentTransform);
			};
			updateTransforms(this, glm::mat4());

			fortree([this, &elapsedTime](Node* n)
			{
				if (!n->enabled)
					return;
				for (Component* c : n->components)
					c->postUpdate(*this);
			});
			//updateTransforms(this, glm::mat4()); //TODO: optimize this, don't do the entire tree, only dynamic objects?

		}

		
		

		void Scene::reset()
		{
			while (getFirstChild())
				delete getFirstChild();

			renderables.clear();
			lights.clear();
			cameraNode = nullptr;
		}



		bool Scene::testBodyCollision(Node * n1, Node * n2)
		{
			if (!n1 || !n2)
				return false;
			if (!n1->rigidBody || !n2->rigidBody)
				return false; // needs a rigidbody for collision testing
			if (!n1->getComponent<vrlib::tien::components::RigidBody>()->actor || !n2->getComponent<vrlib::tien::components::RigidBody>()->actor)
				return false;


			/*CollisionTest test;
			world->contactPairTest(n1->getComponent<vrlib::tien::components::RigidBody>()->body,
				n2->getComponent<vrlib::tien::components::RigidBody>()->body,
				test);
			return test.collision;*/
			return false;
		}

		void Scene::castRay(const math::Ray & ray, std::function<bool(Node* node, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback, bool physics) const
		{
			castRay(ray, [&ray, &callback](Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
			{
				return callback(node, hitPosition, hitNormal);
			});
		}

		std::pair<Node*, glm::vec3> Scene::castRay(const math::Ray & ray, bool physics, const std::function<bool(vrlib::tien::Node*)> &filter) const
		{
			float closest = 99999;
			std::pair<Node*, glm::vec3> ret(nullptr, glm::vec3(0, 0, 0));

			castRay(ray, [&](Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
			{
				if (!filter(node))
					return true;
				if (hitFraction < closest)
				{
					closest = hitFraction;
					ret.first = node;
					ret.second = hitPosition;
				}
				return true;
			}, physics);
			return ret;
		}

		void Scene::castRay(const math::Ray & ray, std::function<bool(Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback, bool physics) const
		{
			if (physics)
			{
/*				class Callback : public btCollisionWorld::RayResultCallback
				{
					std::function<bool(Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback;
					const math::Ray& ray;
				public:
					Callback(std::function<bool(Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback, const math::Ray& ray) : ray(ray)
					{
						this->callback = callback;
					}
					virtual	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) override
					{
						btVector3 hitNormalWorld;
						if (normalInWorldSpace)
							hitNormalWorld = rayResult.m_hitNormalLocal;
						else
							hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;


						bool cont = this->callback(
							(Node*)rayResult.m_collisionObject->getUserPointer(),
							rayResult.m_hitFraction * 1000,
							ray.mOrigin + 1000 * rayResult.m_hitFraction * ray.mDir,
							glm::vec3(hitNormalWorld.x(), hitNormalWorld.y(), hitNormalWorld.z()));
						return cont ? rayResult.m_hitFraction : 1.0f;
					}

				};
				Callback _callback(callback, ray);
				glm::vec3 worldTarget = ray.mOrigin + 1000.0f * ray.mDir;
				//world->rayTest(btVector3(ray.mOrigin.x, ray.mOrigin.y, ray.mOrigin.z), btVector3(worldTarget.x, worldTarget.y, worldTarget.z), _callback);*/
			}
			else
			{
				this->fortree([callback, &ray](const vrlib::tien::Node* node)
				{
					if (!node->transform)
						return;

					vrlib::math::Ray inverseRay = glm::inverse(node->transform->globalTransform) * ray;
					vrlib::CollisionMesh* model = nullptr;
					{
						vrlib::tien::components::ModelRenderer* renderer = node->getComponent<vrlib::tien::components::ModelRenderer>();
						if (renderer)
							model = renderer->model;
					}
					{
						vrlib::tien::components::AnimatedModelRenderer* renderer = node->getComponent<vrlib::tien::components::AnimatedModelRenderer>();
						if (renderer)
							model = renderer->model;
					}
					{
						vrlib::tien::components::MeshRenderer* renderer = node->getComponent<vrlib::tien::components::MeshRenderer>();
						if (renderer)
							model = renderer->mesh;
					}

					if (model)
					{
						std::vector<float> collisions = model->collisionFractions(inverseRay);
						for (float& f : collisions)
						{
							callback(const_cast<vrlib::tien::Node*>(node), f, ray.mOrigin + f * ray.mDir, glm::vec3(0, 0, 0));
						}
					}


				});
			}
		}





/*		void DebugDraw::flush()
		{
			verts.clear();
		}

		void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
		{
			verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(from.x(), from.y(), from.z()), glm::vec4(color.x(), color.y(), color.z(), 1)));
			verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(to.x(), to.y(), to.z()), glm::vec4(color.x(), color.y(), color.z(), 1)));
		}
		void DebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
		{
			verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(PointOnB.x(), PointOnB.y(), PointOnB.z()), glm::vec4(color.x(), color.y(), color.z(), 1)));
			verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(PointOnB.x() + .1 * normalOnB.x(), PointOnB.y() + .1 * normalOnB.y(), PointOnB.z() + .1 * normalOnB.z()), glm::vec4(color.x(), color.y(), color.z(), 1)));
		}
		*/

	}
}