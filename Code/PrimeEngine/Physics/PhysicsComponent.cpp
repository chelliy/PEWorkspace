#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/Mesh.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "PrimeEngine/Scene/DebugRenderer.h"

namespace PE {
namespace Components {

	PE_IMPLEMENT_CLASS1(PhysicsComponent, Component);

	PhysicsComponent::PhysicsComponent(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Handle hMeshInstance, Handle hSceneNode, bool isStatic)
			:Component(context, arena, hMyself)
		{
			// initial sn
			m_hSN = hSceneNode;
			m_hMinst = hMeshInstance;
			SceneNode* pSN = m_hSN.getObject<SceneNode>();

			// initial mesh and aabb
			MeshInstance* pmins = m_hMinst.getObject<MeshInstance>();
			m_hMesh = pmins->m_hAsset;
			Mesh* pMesh = m_hMesh.getObject<Mesh>();

			m_yOffset = abs((pMesh->m_AABB_Max - pMesh->m_AABB_Min).m_y) / 2;

			m_radius = abs((pMesh->m_AABB_Max - pMesh->m_AABB_Min).m_y) / 2;

			m_base = pSN->m_worldTransform * pSN->m_base;
			m_correction = Vector3(0.f, 0.f, 0.f);


			m_isStatic = isStatic;

			m_onGround = false;

			PhysicsManager::Instance()->addComponent(m_hMyself);
		}

	void PhysicsComponent::addDefaultComponents()
		{
			Component::addDefaultComponents();
		}

	void PhysicsComponent::UpdatePosition()
		{
			m_correction = Vector3(0, 0, 0);
			if (!m_isStatic)
			{
				SceneNode* pSN = m_hSN.getObject<SceneNode>();
				if ((m_base.getPos() - (pSN->m_worldTransform * pSN->m_base).getPos()).lengthSqr() <= 0) {

					OutputDebugStringA("Something wrong\n");
				}
				m_base = pSN->m_worldTransform * pSN->m_base;
				m_onGround = false;
				OutputDebugStringA("Position Updated\n");
			}
		}

	void PhysicsComponent::OnCollision(PhysicsComponent* other_PC)
		{
			if (!m_isStatic && other_PC->m_isStatic)
			{
				OutputDebugStringA("Test Collide with static obj\n");
				Collision_Check_Sphere_To_Box(other_PC);
			}
			else {
				OutputDebugStringA("Test Collide with other soldier triggered\n");
			}

		}

	void PhysicsComponent::Collision_Check_Sphere_To_Box(PhysicsComponent* other_PC)
		{
			
			OutputDebugStringA(other_PC->m_hMinst.getObject<MeshInstance>()->m_hAsset.m_dbgName);
			Vector3 closestPoint = ClosestPointOnAABBtoSphere(other_PC);

			Vector3 center = m_base.getPos();

			center.m_y += m_yOffset;

			Vector3 direction = closestPoint - center;

			float squareDistance = direction.lengthSqr();

			//if (m_base.getPos().m_y == 0) {
			//	OutputDebugStringA("soldier base center is at foot\n");
			//}

			//if (other_PC->m_base.getPos().m_y == 0) {
			//	OutputDebugStringA("car base center is at foot\n");
			//}
			//if (other_PC->m_hMesh.getObject<Mesh>()->m_AABB_Min.m_y <=0) {
			//	OutputDebugStringA("car aabb min y offse is smaller or equal to zero\n");
			//}
			//if (closestPoint.m_y == 0) {
			//	OutputDebugStringA("cotact point to car is also at ground\n");
			//}

			if (squareDistance <= m_radius * m_radius)
			{
				OutputDebugStringA("Collide happended\n");

				if (closestPoint.m_y < m_base.getPos().m_y + m_yOffset)
				{
					OutputDebugStringA("Is Ground\n");
					m_onGround = true;
					return;
				}

				if (squareDistance != 0) {
					direction.normalize();
				}
				m_correction += Vector3(direction.m_x, 0, direction.m_z);
			}

		}

	Vector3 PhysicsComponent::ClosestPointOnAABBtoSphere(PhysicsComponent* other_PC)
	{
		//Vector3 curMax = other_PC->m_base.getPos() + other_PC->m_hMesh.getObject<Mesh>()->m_AABB_Max;
		//Vector3 curMin = other_PC->m_base.getPos() + other_PC->m_hMesh.getObject<Mesh>()->m_AABB_Min;
		Vector3 curMax = other_PC->m_hMesh.getObject<Mesh>()->m_AABB_Max;
		Vector3 curMin = other_PC->m_hMesh.getObject<Mesh>()->m_AABB_Min;

		//curMax.m_y += other_PC->m_yOffset;
		//curMin.m_y += other_PC->m_yOffset;

		Vector3 center = m_base.getPos();

		center.m_y += m_yOffset;

		center = other_PC->m_base.inverse() * center;

		Vector3 result = Vector3(max(curMin.m_x, min(center.m_x, curMax.m_x)), max(curMin.m_y, min(center.m_y, curMax.m_y)), max(curMin.m_z, min(center.m_z, curMax.m_z)));

		result = other_PC->m_base * result;

		return result;

	}

}; // namespace Components
}; // namespace PE