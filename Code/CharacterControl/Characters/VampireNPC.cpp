#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/RootSceneNode.h"

#include "VampireNPC.h"
#include "VampireNPCAnimationSM.h"
//#include "SoldierNPCMovementSM.h"
//#include "SoldierNPCBehaviorSM.h"

#include "PrimeEngine/Physics/PhysicsComponent.h"

using namespace PE;
using namespace PE::Components;
using namespace CharacterControl::Events;

namespace CharacterControl {
	namespace Components {

		PE_IMPLEMENT_CLASS1(VampireNPC, Component);

		VampireNPC::VampireNPC(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself, Event_CreateVampireNPC *pEvt) : Component(context, arena, hMyself)
		{

			// hierarchy of soldier and replated components and variables (note variables are just variables, they are not passed events to)
			// scene
			// +-components
			//   +-soldier scene node
			//   | +-components
			//   |   +-soldier skin
			//   |     +-components
			//   |       +-soldier animation state machine
			//   |       +-soldier weapon skin scene node
			//   |         +-components
			//   |           +-weapon mesh

			// game objects
			// +-components
			//   +-soldier npc
			//     +-variables
			//     | +-m_hMySN = soldier scene node
			//     | +-m_hMySkin = skin
			//     | +-m_hMyGunSN = soldier weapon skin scene node
			//     | +-m_hMyGunMesh = weapon mesh
			//     +-components
			//       +-soldier scene node (restricted to no events. this is for state machines to be able to locate the scene node)
			//       +-movement state machine
			//       +-behavior state machine


			// need to acquire redner context for this code to execute thread-safe
			m_pContext->getGPUScreen()->AcquireRenderContextOwnership(pEvt->m_threadOwnershipMask);

			PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
			SceneNode* pMainSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
			pMainSN->addDefaultComponents();

			pMainSN->m_base.setPos(pEvt->m_pos);
			pMainSN->m_base.setU(pEvt->m_u);
			pMainSN->m_base.setV(pEvt->m_v);
			pMainSN->m_base.setN(pEvt->m_n);


			RootSceneNode::Instance()->addComponent(hSN);

			// add the scene node as component of soldier without any handlers. this is just data driven way to locate scnenode for soldier's components
			{
				static int allowedEvts[] = { 0 };
				addComponent(hSN, &allowedEvts[0]);
			}

			int numskins = 1; // 8
			for (int iSkin = 0; iSkin < numskins; ++iSkin)
			{
				float z = (iSkin / 4) * 1.5f;
				float x = (iSkin % 4) * 1.5f;
				PE::Handle hSceneNode("SCENE_NODE", sizeof(SceneNode));
				SceneNode* pSceneNode = new(hSceneNode) SceneNode(*m_pContext, m_arena, hSceneNode);
				pSceneNode->addDefaultComponents();

				pSceneNode->m_base.setPos(Vector3(x, 0, z));

				// rotation scene node to rotate soldier properly, since soldier from Maya is facing wrong direction
				PE::Handle hRotateSN("SCENE_NODE", sizeof(SceneNode));
				SceneNode* pRotateSN = new(hRotateSN) SceneNode(*m_pContext, m_arena, hRotateSN);
				pRotateSN->addDefaultComponents();

				pSceneNode->addComponent(hRotateSN);

				//pRotateSN->m_base.turnLeft(3.1415);

				PE::Handle hVampireAnimSM("VampireNPCAnimationSM", sizeof(VampireNPCAnimationSM));
				VampireNPCAnimationSM* pVampireAnimSM = new(hVampireAnimSM) VampireNPCAnimationSM(*m_pContext, m_arena, hVampireAnimSM);
				pVampireAnimSM->addDefaultComponents();

				pVampireAnimSM->m_debugAnimIdOffset = 0;// rand() % 3;

				PE::Handle hSkeletonInstance("SkeletonInstance", sizeof(SkeletonInstance));
				SkeletonInstance* pSkelInst = new(hSkeletonInstance) SkeletonInstance(*m_pContext, m_arena, hSkeletonInstance,
					hVampireAnimSM);
				pSkelInst->addDefaultComponents();

				pSkelInst->initFromFiles("vampire-t-pose_Hips.skela", "Vampire", pEvt->m_threadOwnershipMask);

				pSkelInst->setAnimSet("vampire-action-adventure-pack_Hips.animseta", "Vampire");

				PE::Handle hMeshInstance("MeshInstance", sizeof(MeshInstance));
				MeshInstance* pMeshInstance = new(hMeshInstance) MeshInstance(*m_pContext, m_arena, hMeshInstance);
				pMeshInstance->addDefaultComponents();

				pMeshInstance->initFromFile(pEvt->m_meshFilename, pEvt->m_package, pEvt->m_threadOwnershipMask);

				pSkelInst->addComponent(hMeshInstance);

				// add skin to scene node
				pRotateSN->addComponent(hSkeletonInstance);

			//	//add physics
			//PE:Handle hPhysicsComponent("PhysicsComponent", sizeof(PhysicsComponent));
			//	PhysicsComponent* pPhysicsComponent = new(hPhysicsComponent) PhysicsComponent(*m_pContext, m_arena, hPhysicsComponent, hMeshInstance, hSceneNode, false);
			//	pPhysicsComponent->addDefaultComponents();

			//	pMainSN->m_hPhysicsComponent = hPhysicsComponent;

				pMainSN->addComponent(hSceneNode);
			}

			m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(pEvt->m_threadOwnershipMask);

#if 1
			//// add movement state machine to soldier npc
			//PE::Handle hSoldierMovementSM("SoldierNPCMovementSM", sizeof(SoldierNPCMovementSM));
			//SoldierNPCMovementSM* pSoldierMovementSM = new(hSoldierMovementSM) SoldierNPCMovementSM(*m_pContext, m_arena, hSoldierMovementSM);
			//pSoldierMovementSM->addDefaultComponents();

			//// add it to soldier NPC
			//addComponent(hSoldierMovementSM);

			//// add behavior state machine ot soldier npc
			//PE::Handle hSoldierBheaviorSM("SoldierNPCBehaviorSM", sizeof(SoldierNPCBehaviorSM));
			//SoldierNPCBehaviorSM* pSoldierBehaviorSM = new(hSoldierBheaviorSM) SoldierNPCBehaviorSM(*m_pContext, m_arena, hSoldierBheaviorSM, hSoldierMovementSM);
			//pSoldierBehaviorSM->addDefaultComponents();

			//// add it to soldier NPC
			//addComponent(hSoldierBheaviorSM);

			//StringOps::writeToString(pEvt->m_patrolWayPoint, pSoldierBehaviorSM->m_curPatrolWayPoint, 32);
			//pSoldierBehaviorSM->m_havePatrolWayPoint = StringOps::length(pSoldierBehaviorSM->m_curPatrolWayPoint) > 0;

			//// start the soldier
			//pSoldierBehaviorSM->start();
#endif
		}

		void VampireNPC::addDefaultComponents()
		{
			Component::addDefaultComponents();

			// custom methods of this component
		}

	}; // namespace Components
}; // namespace CharacterControl
