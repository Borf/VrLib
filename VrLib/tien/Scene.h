#pragma once

#include "Node.h"

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;


namespace vrlib
{
	namespace tien
	{
		class Scene : public Node
		{
		public:
			Scene();
			Scene(const Scene& other);
			Node* cameraNode;

		private:
			bool treeDirty;
			std::list<Node*> renderables;
			std::list<Node*> lights;

			virtual void setTreeDirty() override;
			void updateRenderables();
			void prepareForRun();
			void update(float elapsedTime);

			friend class Renderer;
			friend class Tien;


			btBroadphaseInterface*                  broadphase;
			btDefaultCollisionConfiguration*        collisionConfiguration;
			btCollisionDispatcher*                  dispatcher;
			btSequentialImpulseConstraintSolver*    solver;
			btDiscreteDynamicsWorld*                world;

		};
	}
}