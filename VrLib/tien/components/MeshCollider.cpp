#include "MeshCollider.h"
#include <VrLib/json.hpp>
#include <VrLib/Model.h>
#include <VrLib/tien/Node.h>
#include <VrLib/tien/Scene.h>
#include <VrLib/tien/components/ModelRenderer.h>
#include <VrLib/tien/components/Transform.h>
#include <VrLib/tien/components/RigidBody.h>

namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			MeshCollider::MeshCollider(Node* node, bool convex)
			{
				this->convex = convex;
				buildShape(node);
			}


			void MeshCollider::buildShape(Node* node)
			{
				Model* model = node->getComponent<ModelRenderer>()->model;
				glm::vec3 centerOfGravity = model->aabb.center();;
				auto verts = model->getIndexedTriangles();

				if (node->rigidBody)
				{
					if (node->rigidBody->getMass() > 0)
						convex = true;
				}

				glm::vec3 scale = node->transform->getGlobalScale();
				//this->offset = scale * centerOfGravity;
				for (size_t i = 0; i < verts.second.size(); i++)
					verts.second[i] *= scale;


				if (convex)
				{
					physx::PxConvexMeshDesc convexDesc;
					convexDesc.points.count = verts.second.size();;
					convexDesc.points.stride = sizeof(physx::PxVec3);
					convexDesc.points.data = &verts.second[0];
					convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
					convexDesc.vertexLimit = 255;

					physx::PxDefaultMemoryOutputStream buf;
					if (!node->getScene().gCooking->cookConvexMesh(convexDesc, buf))
						throw "oops";

					physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
					physx::PxConvexMesh* convexMesh = node->getScene().gPhysics->createConvexMesh(input);

					shape = node->getScene().gPhysics->createShape(physx::PxConvexMeshGeometry(convexMesh), *node->getScene().gMaterial);

				}
				else
				{
					physx::PxTriangleMeshDesc meshDesc;
					meshDesc.points.count = verts.second.size();
					meshDesc.points.stride = sizeof(physx::PxVec3);
					meshDesc.points.data = new physx::PxVec3[verts.second.size()];
					memcpy((void*)meshDesc.points.data, &verts.second[0], sizeof(physx::PxVec3) * verts.second.size());

					meshDesc.triangles.count = verts.first.size()/3;
					meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
//					meshDesc.triangles.data = &verts.first[0];
					meshDesc.triangles.data = new physx::PxU32[verts.first.size()];
					memcpy((void*)meshDesc.triangles.data, &verts.first[0], sizeof(physx::PxU32) * verts.first.size());

					physx::PxDefaultMemoryOutputStream writeBuffer;
//					physx::PxTriangleMeshCookingResult::Enum result;
					bool status = node->getScene().gCooking->cookTriangleMesh(meshDesc, writeBuffer);
					if (!status)
						throw "oops";

					physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
					auto mesh = node->getScene().gPhysics->createTriangleMesh(readBuffer);
					shape = node->getScene().gPhysics->createShape(physx::PxTriangleMeshGeometry(mesh), *node->getScene().gMaterial);
				}
			}


			physx::PxShape* MeshCollider::getShape(physx::PxPhysics* physics, const glm::vec3 &scale)
			{
				return shape;
			}

			json MeshCollider::toJson(json & meshes) const
			{
				json ret;

				ret["type"] = "collider";
				ret["collider"] = "mesh";
				ret["convex"] = convex;
				return ret;
			}

			void MeshCollider::buildEditor(EditorBuilder * builder, bool folded)
			{
				builder->addTitle("Mesh Collider");

				builder->beginGroup("Convex", false);
				builder->addCheckbox(convex, [this](bool newValue)
				{
					this->convex = newValue;
				});
				builder->endGroup();


			}

		}
	}
}