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
				components::Renderable* renderable;
				if (renderable = n->getComponent<components::Renderable>())
				{
					renderables.push_back(n);
					if (renderContexts.find(renderable->renderContext) == renderContexts.end())
					{
						renderable->renderContext->init(); // TODO: move this to another place?
						renderContexts.insert(renderable->renderContext);
					}
				}
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
			world->setDebugDrawer(debugDrawer = new DebugDraw());
			debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);


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