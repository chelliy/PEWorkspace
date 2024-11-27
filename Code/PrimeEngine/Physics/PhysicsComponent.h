#ifndef __PE_PHYSICSCOMPONENT_H__
#define __PE_PHYSICSCOMPONENT_H__


#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/MemoryManagement/Handle.h"

namespace PE {
namespace Components {

	struct PhysicsComponent : public Component
	{
		PE_DECLARE_CLASS(PhysicsComponent);

		PhysicsComponent(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Handle hMeshInstance, Handle hSceneNode, bool isStatic);
		virtual ~PhysicsComponent() {}

		virtual void addDefaultComponents();
		void UpdatePosition();
		void OnCollision(PhysicsComponent* other_PC);
		void Collision_Check_Sphere_To_Box(PhysicsComponent* other_PC);
		Vector3 ClosestPointOnAABBtoSphere(PhysicsComponent* other_PC);

		Handle m_hMesh; // handle to mesh
		Handle m_hSN; // handle to scene node
		Handle m_hMinst; // handle to mesh instance

		Matrix4x4 m_base;
		Vector3 m_correction;
		float m_yOffset;
		float m_radius;
		bool m_isStatic;
		bool m_onGround;
	};

}; // namespace Components
}; // namespace PE
#endif