#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "PhysicsManager.h"
#include "PhysicsComponent.h"

#include "PrimeEngine/Scene/DebugRenderer.h"

namespace PE {
	namespace Components
	{
		Handle PhysicsManager::s_hInstance;

		PE_IMPLEMENT_CLASS1(PhysicsManager, Component);

		PhysicsManager::PhysicsManager(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
			: Component(context, arena, hMyself)
			, m_PhysicsComponents(context, arena, 64)
		{}

		void PhysicsManager::addComponent(Handle hComponent, int* pAllowedEvents)
		{
			if (hComponent.getObject<Component>()->isInstanceOf<PhysicsComponent>())
			{
				m_PhysicsComponents.add(hComponent);
			}

			Component::addComponent(hComponent, pAllowedEvents);
		}

		void PhysicsManager::addDefaultComponents()
		{
			PE_REGISTER_EVENT_HANDLER(PE::Events::Event_PHYSICS_START, PhysicsManager::do_PHYSICS_START);
			PE_REGISTER_EVENT_HANDLER(PE::Events::Event_PHYSICS_END, PhysicsManager::do_PHYSICS_END);
			PE_REGISTER_EVENT_HANDLER(PE::Events::Event_UPDATE, PhysicsManager::do_UPDATE);
			PE_REGISTER_EVENT_HANDLER(PE::Events::Event_PRE_RENDER_needsRC, PhysicsManager::do_PRE_RENDER_needsRC);

			Component::addDefaultComponents();
		}

		void PhysicsManager::Construct(PE::GameContext& context, PE::MemoryArena arena)
		{
			Handle h("PHYSICS_MANAGER", sizeof(PhysicsManager));
			PhysicsManager* pPhysicsManager = new(h) PhysicsManager(context, arena, h);
			pPhysicsManager->addDefaultComponents();
			SetInstance(h);
		}

		void PhysicsManager::do_PHYSICS_START(PE::Events::Event* pEvt) {
			//for (int i = 0; i < m_PhysicsComponents.m_size; i++) {
			//	PhysicsComponent* pPC = m_PhysicsComponents[i].getObject<PhysicsComponent>();
			//	for (int j = 0; j < m_PhysicsComponents.m_size; j++) {
			//		if (pPC->m_isStatic || i == j) continue;
			//		PhysicsComponent* pPC_other = m_PhysicsComponents[j].getObject<PhysicsComponent>();
			//		pPC->OnCollision(pPC_other);
			//	}
			//}
		}

		void PhysicsManager::do_UPDATE(PE::Events::Event* pEvt) {

		}

		void PhysicsManager::do_PHYSICS_END(PE::Events::Event* pEvt) {
			//for (int i = 0; i < m_PhysicsComponents.m_size; i++) {
			//	PhysicsComponent* pPC = m_PhysicsComponents[i].getObject<PhysicsComponent>();
			//	pPC->UpdatePosition();
			//}
		}

		void PhysicsManager::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt) {
			for (int i = 0; i < m_PhysicsComponents.m_size; i++) {
				PhysicsComponent* pPC = m_PhysicsComponents[i].getObject<PhysicsComponent>();
				DebugRenderer::Instance()->createAABBMesh(pPC->m_hMesh.getObject<Mesh>(), pPC->m_hSN.getObject<SceneNode>()->m_worldTransform, 0.1);

				//Vector3 curMax = pPC->m_hMesh.getObject<Mesh>()->m_AABB_Max;
				//Vector3 curMin = pPC->m_hMesh.getObject<Mesh>()->m_AABB_Min;
				////curMax.m_y += pPC->m_yOffset;
				////curMin.m_y += pPC->m_yOffset;

				////Vector3 curMax = pPC->m_base.getPos();
				////curMax .m_y +=pPC->m_yOffset;
				////Vector3 curMin = pPC->m_base.getPos() + pPC->m_hMesh.getObject<Mesh>()->m_AABB_Min;
				//DebugRenderer::Instance()->createALine(curMin, curMax, 0.1, Vector3(1, 0, 0));
				//DebugRenderer::Instance()->createALine(curMax, curMin, 0.1, Vector3(0, 0, 1));
				////curMax = pPC->m_hMesh.getObject<Mesh>()->m_world_AABB_Max;
				////curMin = pPC->m_hMesh.getObject<Mesh>()->m_world_AABB_Min;
				////DebugRenderer::Instance()->createALine(curMax, curMin, 0.1);
			}
		}
	};
};