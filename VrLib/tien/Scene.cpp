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

			gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, physx::PxCookingParams(gPhysics->getTolerancesScale()));
			if (!gCooking)
				throw("PxCreateCooking failed!");

			physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
			if (pvdClient)
			{
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
				pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
			}
			gMaterial = gPhysics->createMaterial(0.2f, 0.2f, 0.0f);
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
			allRenderables.clear();
			lights.clear();
			fortree([this](Node* n)
			{
				if (!n->renderAbles.empty())
					allRenderables.insert(allRenderables.end(), n->renderAbles.begin(), n->renderAbles.end());
				if (n->getComponent<components::Light>())
					lights.push_back(n);
			});
			allRenderables.sort([](components::Renderable* a, components::Renderable* b) { return (int)a->renderContextDeferred < (int)b->renderContextDeferred; });

			for (auto renderable : allRenderables)
			{
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
				while (physicsTimer > 0)
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
				if (n->transform)
				{
					n->transform->buildTransform(parentTransform);					
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
		}

		
		

		void Scene::reset()
		{
			while (getFirstChild())
				delete getFirstChild();

			allRenderables.clear();
			lights.clear();
			cameraNode = nullptr;
		}



		bool Scene::testBodyCollision(Node * n1, Node * n2)
		{
			if (!n1 || !n2)
				return false;
			if (!n1->rigidBody || !n2->rigidBody)
				return false; // needs a rigidbody for collision testing
			if (!n1->rigidBody->actor || !n2->rigidBody->actor)
				return false;

			int countA = n1->rigidBody->actor->getNbShapes();
			physx::PxShape** shapeA = new physx::PxShape*[countA];
			n1->rigidBody->actor->getShapes(shapeA, countA, 0);

			int countB = n2->rigidBody->actor->getNbShapes();
			physx::PxShape** shapeB = new physx::PxShape*[countB];
			n2->rigidBody->actor->getShapes(shapeB, countB, 0);
			
			for(int a = 0; a < countA; a++)
				for(int b = 0; b < countB; b++)
					if (physx::PxGeometryQuery::overlap(shapeA[a]->getGeometry().any(), physx::PxShapeExt::getGlobalPose(*shapeA[a], *n1->rigidBody->actor),
														shapeB[b]->getGeometry().any(), physx::PxShapeExt::getGlobalPose(*shapeB[b], *n2->rigidBody->actor)))
						return true;

			return false;
			

			/*CollisionTest test;
			world->contactPairTest(n1->getComponent<vrlib::tien::components::RigidBody>()->body,
				n2->getComponent<vrlib::tien::components::RigidBody>()->body,
				test);
			return test.collision;*/
			return false;
		}

		bool Scene::testBodyCollision(Node * n1, const physx::PxGeometry &shapeB, const physx::PxTransform &transformB)
		{
			if (!n1)
				return false;
			if (!n1->rigidBody)
				return false; // needs a rigidbody for collision testing
			if (!n1->rigidBody->actor)
				return false;

			int countA = n1->rigidBody->actor->getNbShapes();
			physx::PxShape** shapeA = new physx::PxShape*[countA];
			n1->rigidBody->actor->getShapes(shapeA, countA, 0);

			for (int a = 0; a < countA; a++)
				if (physx::PxGeometryQuery::overlap(shapeA[a]->getGeometry().any(), physx::PxShapeExt::getGlobalPose(*shapeA[a], *n1->rigidBody->actor),
					shapeB, transformB))
					return true;

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

		std::pair<Node*, glm::vec3> Scene::castRay(const math::Ray & ray, bool physics, const std::function<CollisionFilter(const vrlib::tien::Node*)> &filter) const
		{
			float closest = 99999;
			std::pair<Node*, glm::vec3> ret(nullptr, glm::vec3(0, 0, 0));

			castRay(ray, [&](Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)
			{
				if (filter(node) == CollisionFilter::Ignore)
					return true;
				if (hitFraction < closest)
				{
					closest = hitFraction;
					ret.first = node;
					ret.second = hitPosition;
				}
				return true;
			}, physics, filter);
			return ret;
		}

		void Scene::castRay(const math::Ray & ray, std::function<bool(Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback, bool physics, std::function<CollisionFilter(const vrlib::tien::Node*)> filter) const
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
				this->fortree([callback, &ray, &filter](const vrlib::tien::Node* node)
				{
					if (!node->transform)
						return;

					CollisionFilter filterResult = CollisionFilter::Include;

					if (filter)
						filterResult = filter(node);
					if(filterResult == CollisionFilter::Ignore)
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
						if (filterResult == CollisionFilter::IgnorePolygon)
						{
							float f = model->aabb.getRayCollision(inverseRay, 0, 10000);
							if (f >= 0)
							{
								glm::vec3 collision = inverseRay.mOrigin + f * inverseRay.mDir;
								callback(const_cast<vrlib::tien::Node*>(node), f, collision, glm::vec3(0, 0, 0));
							}
						}
						else
							model->collisionFractions(inverseRay, [&callback, &node, &ray](float f)
							{
								return callback(const_cast<vrlib::tien::Node*>(node), f, ray.mOrigin + f * ray.mDir, glm::vec3(0, 0, 0));
							});
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