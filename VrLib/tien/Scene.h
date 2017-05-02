#pragma once

#include "Node.h"
#include <list>
#include <set>
#include <VrLib/gl/Vertex.h>
#include <PxPhysicsAPI.h>
#include "components/Renderable.h"
#include <VrLib/math/Frustum.h>

#ifndef _DEBUG
//#pragma comment(lib, "PhysX3GpuCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CHECKED_x86.lib")
#pragma comment(lib, "PhysX3CookingCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CommonCHECKED_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicCHECKED_x86.lib")
#pragma comment(lib, "PhysX3VehicleCHECKED.lib")
#pragma comment(lib, "PhysX3ExtensionsCHECKED.lib")
#pragma comment(lib, "PxTaskCHECKED_x86.lib")
#pragma comment(lib, "PxPvdSDKCHECKED_x86.lib")
#pragma comment(lib, "PxFoundationCHECKED_x86.lib")
#pragma comment(lib, "PxPvdSDKCHECKED_x86.lib")
#pragma comment(lib, "PxTaskCHECKED_x86.lib")
#else
//#pragma comment(lib, "PhysX3GpuDEBUG_x86.lib")
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib, "PhysX3VehicleDEBUG.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PxTaskDEBUG_x86.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "PxTaskDEBUG_x86.lib")
#endif


namespace vrlib
{
	namespace math { class Ray; }
	namespace tien
	{
		namespace components { class Light;  }

		class Scene : public Node
		{
		public:
			Scene();
			Node* cameraNode;

		private:
			Scene(const Scene& other);

			bool treeDirty;
			std::set<components::Renderable::RenderContext*> renderContextsDeferred;
			std::set<components::Renderable::RenderContext*> renderContextsShadow;
			std::set<components::Renderable::RenderContext*> renderContextsForward;
			std::list<Node*> renderables;
			std::vector<Node*> lights;
			math::Frustum* frustum;


			void addRigidBody(Node* node);
			void addCollider(Node* node);

			virtual Scene &getScene() override { return *this; }
			virtual void setTreeDirty(Node* newNode, bool isNewNode) override;
			void updateRenderables();
			void update(float elapsedTime);
			void init();

			friend class Renderer;
			friend class Tien;
			friend class Node;
			friend class components::Light;


			const float						physicsRate = 1 / 90.0f; // physics speed
			float							physicsTimer = 0;
			physx::PxDefaultAllocator		gAllocator;
			physx::PxDefaultErrorCallback	gErrorCallback;
			physx::PxFoundation*			gFoundation = nullptr;
			physx::PxDefaultCpuDispatcher*	gDispatcher = nullptr;
			physx::PxPvd*					gPvd = nullptr;
		public:
			physx::PxMaterial*				gMaterial = nullptr;
			physx::PxPhysics*				gPhysics = nullptr;
			physx::PxScene*					gScene = nullptr;
			physx::PxCooking*				gCooking = nullptr;

			void reset();

			bool testBodyCollision(Node* n1, Node* n2);
			bool testBodyCollision(Node* n1, const physx::PxGeometry&, const physx::PxTransform &);


			/**
			* casts a ray through the scene, using the physics world or the polygon meshes
			* callback is called when a collision occurs. Callback should return true for further results, false for no more results
			*/
			void castRay(const math::Ray& ray, std::function<bool(Node* node, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback, bool physics = true) const;

			/**
			* casts a ray through the scene, using the physics world or the polygon meshes
			* callback is called when a collision occurs. Callback should return true for further results, false for no more results. Fraction is a fraction of the ray, based on a normalized ray
			*/
			void castRay(const math::Ray& ray, std::function<bool(Node* node, float hitFraction, const glm::vec3 &hitPosition, const glm::vec3 &hitNormal)> callback, bool physics = true) const;
			std::pair<Node*, glm::vec3> castRay(const math::Ray& ray, bool physics = true, const std::function<bool(vrlib::tien::Node*)> &filter = [](vrlib::tien::Node*) {return true; }) const;


		};
	}
}