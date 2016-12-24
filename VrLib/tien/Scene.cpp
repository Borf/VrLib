#include "Scene.h"
#include "components/AnimatedModelRenderer.h"
#include "components/ModelRenderer.h"
#include "components/Light.h"
#include "components/Transform.h"
#include "components/Camera.h"
#include "components/RigidBody.h"
#include "components/Collider.h"

#include <VrLib/math/Ray.h>
#include <VrLib/Model.h>

#include <btBulletDynamicsCommon.h>

#include <algorithm>


namespace vrlib
{
	namespace tien
	{

		Scene::Scene() : Node("Root", nullptr)
		{
			cameraNode = nullptr;
			world = nullptr;
			frustum = new math::Frustum();
		}

		Scene::Scene(const Scene& other) : Node("", nullptr)
		{
			throw "Not allowed";
		}

		void Scene::init()
		{
			broadphase = new btDbvtBroadphase();
			collisionConfiguration = new btDefaultCollisionConfiguration();
			dispatcher = new btCollisionDispatcher(collisionConfiguration);
			solver = new btSequentialImpulseConstraintSolver();
			world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
			world->setGravity(btVector3(0, -9.8f, 0));
			world->setDebugDrawer(debugDrawer = new DebugDraw());
			debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		}


		void Scene::addRigidBody(Node* node)
		{
			node->rigidBody->init(world);
		}
		void Scene::addCollider(Node* node)
		{
			if (node->rigidBody)
				node->rigidBody->updateCollider(world);
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

			fortree([this, &elapsedTime](Node* n)
			{
				for (Component* c : n->components)
					c->postUpdate(*this);
			});
			updateTransforms(this, glm::mat4());

		}

		
		

		class CollisionTest : public btCollisionWorld::ContactResultCallback
		{
		public:
			bool collision = false;
			virtual	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
			{
				collision = true;
				return 0;
			}
		};
		bool Scene::testBodyCollision(Node * n1, Node * n2)
		{
			if (!n1 || !n2)
				return false;
			if (!n1->getComponent<vrlib::tien::components::RigidBody>()->body || !n2->getComponent<vrlib::tien::components::RigidBody>()->body)
				return false;


			CollisionTest test;
			world->contactPairTest(n1->getComponent<vrlib::tien::components::RigidBody>()->body,
				n2->getComponent<vrlib::tien::components::RigidBody>()->body,
				test);
			return test.collision;
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
				class Callback : public btCollisionWorld::RayResultCallback
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
				world->rayTest(btVector3(ray.mOrigin.x, ray.mOrigin.y, ray.mOrigin.z), btVector3(worldTarget.x, worldTarget.y, worldTarget.z), _callback);
			}
			else
			{
				this->fortree([callback, &ray](const vrlib::tien::Node* node)
				{
					if (!node->transform)
						return;

					vrlib::math::Ray inverseRay = glm::inverse(node->transform->globalTransform) * ray;
					vrlib::Model* model = nullptr;
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





		void DebugDraw::flush()
		{
			/*if (!verts.empty())
			{
				glLineWidth(2);
				glColor3f(1.0, 0, 0);
				vrlib::gl::setAttributes<vrlib::gl::VertexP3N3T2>(&verts[0]);
				glDrawArrays(GL_LINES, 0, verts.size());
				verts.clear();
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
			}*/
			verts.clear();
		}

		void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
		{
			verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(from.x(), from.y(), from.z()), glm::vec4(color.x(), color.y(), color.z(), 1)));
			verts.push_back(vrlib::gl::VertexP3C4(glm::vec3(to.x(), to.y(), to.z()), glm::vec4(color.x(), color.y(), color.z(), 1)));
		}


	}
}