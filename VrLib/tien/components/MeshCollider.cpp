#include "MeshCollider.h"
#include <VrLib/json.hpp>
#include <VrLib/Model.h>
#include <VrLib/tien/Node.h>
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

				this->offset = node->transform->getGlobalScale() * centerOfGravity;

		/*		if (convex)
				{
					btConvexHullShape* objShape = new btConvexHullShape();
					objShape->setMargin(margin);

					glm::vec3 scale(1, 1, 1);// = node->transform->getGlobalScale();


					for (const glm::vec3 &p : verts.second)
						objShape->addPoint(btVector3(scale.x * (p.x - centerOfGravity.x), scale.y * (p.y - centerOfGravity.y), scale.z * (p.z - centerOfGravity.z)), false);
					objShape->recalcLocalAabb();

					btShapeHull* hull = new btShapeHull(objShape);
					btScalar margin = objShape->getMargin();
					hull->buildHull(0);
					btConvexHullShape* simplifiedConvexShape = new btConvexHullShape((btScalar*)hull->getVertexPointer(), hull->numVertices());
					delete objShape;
					objShape = simplifiedConvexShape;
					delete hull;
					shape = objShape;
					shape->setMargin(margin);
				}
				else
				{
					btVector3* gVertices = new btVector3[verts.second.size()];
					int* gIndices = new int[verts.first.size()];
					for (size_t i = 0; i < verts.second.size(); i++)
						gVertices[i] = btVector3(verts.second[i].x - centerOfGravity.x, verts.second[i].y - centerOfGravity.y, verts.second[i].z - centerOfGravity.z);

					for (size_t i = 0; i < verts.first.size(); i++)
						gIndices[i] = verts.first[i];

					btTriangleIndexVertexArray* m_indexVertexArrays = new btTriangleIndexVertexArray(verts.second.size() / 3,
						gIndices,
						3 * sizeof(int),
						verts.first.size(), (btScalar*)&gVertices[0].x(), sizeof(btVector3));
					btVector3 aabbMin(-100000, -100000, -100000), aabbMax(100000, 100000, 100000);
					btBvhTriangleMeshShape* objShape = new btBvhTriangleMeshShape(m_indexVertexArrays, true, aabbMin, aabbMax);
					shape = objShape;
					shape->setMargin(margin);
				}*/
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