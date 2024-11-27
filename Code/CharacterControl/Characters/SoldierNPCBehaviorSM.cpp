#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "../ClientGameObjectManagerAddon.h"
#include "../CharacterControlContext.h"
#include "SoldierNPCMovementSM.h"
#include "SoldierNPCAnimationSM.h"
#include "SoldierNPCBehaviorSM.h"
#include "SoldierNPC.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Render/IRenderer.h"
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl{

namespace Components{

PE_IMPLEMENT_CLASS1(SoldierNPCBehaviorSM, Component);

SoldierNPCBehaviorSM::SoldierNPCBehaviorSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, PE::Handle hMovementSM) 
: Component(context, arena, hMyself)
, m_hMovementSM(hMovementSM)
{

}

void SoldierNPCBehaviorSM::start()
{
	if (m_havePatrolWayPoint)
	{
		m_state = WAITING_FOR_WAYPOINT; // will update on next do_UPDATE()
	}
	else
	{
		m_state = IDLE; // stand in place

		PE::Handle h("SoldierNPCMovementSM_Event_STOP", sizeof(SoldierNPCMovementSM_Event_STOP));
		SoldierNPCMovementSM_Event_STOP *pEvt = new(h) SoldierNPCMovementSM_Event_STOP();

		m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
		// release memory now that event is processed
		h.release();
		
	}	
}

void SoldierNPCBehaviorSM::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_TARGET_REACHED, SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED);
	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, SoldierNPCBehaviorSM::do_UPDATE);

	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, SoldierNPCBehaviorSM::do_PRE_RENDER_needsRC);
}

// sent by movement state machine whenver it reaches current target
void SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED(PE::Events::Event *pEvt)
{
	PEINFO("SoldierNPCBehaviorSM::do_SoldierNPCMovementSM_Event_TARGET_REACHED\n");

	if (m_state == PATROLLING_WAYPOINTS)
	{
		ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
		if (pGameObjectManagerAddon)
		{
			// search for waypoint object
			WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
			if (pWP && StringOps::length(pWP->m_nextWayPointName) > 0)
			{

				SoldierNPCMovementSM_Event_TARGET_REACHED* pRealEvt = (SoldierNPCMovementSM_Event_TARGET_REACHED*)(pEvt);
				if (pRealEvt->m_takingCover) {
					m_state = PATROLLING_WAYPOINTS;
					PE::Handle h("SoldierNPCMovementSM_Event_TAKE_COVER", sizeof(SoldierNPCMovementSM_Event_TAKE_COVER));
					Events::SoldierNPCMovementSM_Event_TAKE_COVER* pEvt = new(h) SoldierNPCMovementSM_Event_TAKE_COVER();

					m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
					h.release();
				}
				else {
					// have next waypoint to go to
					pWP = pGameObjectManagerAddon->getWayPoint(pWP->m_nextWayPointName);
					if (pWP)
					{
						//if (pGameObjectManagerAddon->getWayPoint(pWP->m_nextWayPointName)) 
						//{
						//	if (rand() % 2 > 0) 
						//	{
						//		pWP = pGameObjectManagerAddon->getWayPoint(pWP->m_nextWayPointName);
						//	}
						//}
						StringOps::writeToString(pWP->m_name, m_curPatrolWayPoint, 32);

						PEINFO("Got IN\n");
						m_state = PATROLLING_WAYPOINTS;
						PE::Handle h("SoldierNPCMovementSM_Event_MOVE_TO", sizeof(SoldierNPCMovementSM_Event_MOVE_TO));
						Events::SoldierNPCMovementSM_Event_MOVE_TO* pEvt = new(h) SoldierNPCMovementSM_Event_MOVE_TO(pWP->m_base.getPos());

						pEvt->m_runniung = pWP->m_needToRunToThisWaypoint > 0;
						pEvt->m_takeCover = pWP->m_takingCover > 0;
						pEvt->m_highCover = pWP->m_highCover > 0;
						pEvt->m_jumpStart = pWP->m_jumpStart > 0;
						pEvt->m_jumpEnd = pWP->m_jumpEnd > 0;
						pEvt->m_preStartDistance = pWP->m_preStartDistance;

						m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
						// release memory now that event is processed
						h.release();
					}

					else
					{
						m_state = IDLE;
						//m_state = STAND_SHOOT;

						//SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
						//PE::Handle hSoldierSceneNode = pSol->getFirstComponentHandle<PE::Components::SceneNode>();
						//Matrix4x4 base = hSoldierSceneNode.getObject<PE::Components::SceneNode>()->m_worldTransform;


						//PE::Handle h("SoldierNPCMovementSM_Event_STAND_SHOOT", sizeof(SoldierNPCMovementSM_Event_STAND_SHOOT));
						//Events::SoldierNPCMovementSM_Event_STAND_SHOOT* pEvt = new(h) SoldierNPCMovementSM_Event_STAND_SHOOT(Vector3(m_curTargetX, 0, m_curTargetZ));

						//m_hMovementSM.getObject<Component>()->handleEvent(pEvt);

						//h.release();


						// no need to send the event. movement state machine will automatically send event to animation state machine to play idle animation
					}
				}
			}
		}
	}
}

// this event is executed when thread has RC
void SoldierNPCBehaviorSM::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt)
{
	Event_PRE_RENDER_needsRC *pRealEvent = (Event_PRE_RENDER_needsRC *)(pEvt);
	if (m_havePatrolWayPoint)
	{
		char buf[80];
		sprintf(buf, "Patrol Waypoint: %s",m_curPatrolWayPoint);
		SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
		PE::Handle hSoldierSceneNode = pSol->getFirstComponentHandle<PE::Components::SceneNode>();
		Matrix4x4 base = hSoldierSceneNode.getObject<PE::Components::SceneNode>()->m_worldTransform;
		
		DebugRenderer::Instance()->createTextMesh(
			buf, false, false, true, false, 0,
			base.getPos(), 0.01f, pRealEvent->m_threadOwnershipMask);
		
		{
			//we can also construct points ourself
			bool sent = false;
			ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
			if (pGameObjectManagerAddon)
			{
				WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
				if (pWP)
				{
					Vector3 target = pWP->m_base.getPos();
					Vector3 pos = base.getPos();
					Vector3 color(1.0f, 1.0f, 0);
					Vector3 linepts[] = {pos, color, target, color};
					
					DebugRenderer::Instance()->createLineMesh(true, base,  &linepts[0].m_x, 2, 0);// send event while the array is on the stack
					sent = true;
				}
			}
			if (!sent) // if for whatever reason we didnt retrieve waypoint info, send the event with transform only
				DebugRenderer::Instance()->createLineMesh(true, base, NULL, 0, 0);// send event while the array is on the stack
		}
	}

	//if (m_state == STAND_SHOOT) {
	//	char buf[80];
	//	sprintf(buf, "Target");

	//	if (m_curTargetX > 10) {
	//		positiveX = false;
	//	}
	//	else if (m_curTargetX < -10) {
	//		positiveX = true;
	//	}

	//	if (m_curTargetZ > 10) {
	//		positiveZ = false;
	//	}
	//	else if (m_curTargetZ < -10) {
	//		positiveZ = true;
	//	}

	//	if (positiveX) {
	//		m_curTargetX += 0.1f;
	//	}
	//	else {
	//		m_curTargetX -= 0.1f;
	//	}

	//	if (rand() % 2 > 0) {
	//		if (positiveZ) {
	//			m_curTargetZ += 0.1f;
	//		}
	//		else {
	//			m_curTargetZ -= 0.1f;
	//		}
	//	}


	//	Vector3 curTargetPos = { m_curTargetX , 0, m_curTargetZ };

	//	SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
	//	PE::Handle hSoldierSceneNode = pSol->getFirstComponentHandle<PE::Components::SceneNode>();
	//	Matrix4x4 base = hSoldierSceneNode.getObject<PE::Components::SceneNode>()->m_worldTransform;
	//	if ((curTargetPos - base.getPos()).length() > 0.0001) {
	//		curTargetPos.m_x += 1;
	//		curTargetPos.m_z += 1;

	//	}

	//	DebugRenderer::Instance()->createTextMesh(
	//		buf, false, false, true, false, 0,
	//		curTargetPos, 0.01f, pRealEvent->m_threadOwnershipMask);

	//	PE::Handle h("SoldierNPCMovementSM_Event_STAND_SHOOT", sizeof(SoldierNPCMovementSM_Event_STAND_SHOOT));
	//	Events::SoldierNPCMovementSM_Event_STAND_SHOOT* pEvt = new(h) SoldierNPCMovementSM_Event_STAND_SHOOT(curTargetPos);

	//	m_hMovementSM.getObject<Component>()->handleEvent(pEvt);

	//	h.release();


	//}

}

void SoldierNPCBehaviorSM::do_UPDATE(PE::Events::Event *pEvt)
{
	if (m_state == WAITING_FOR_WAYPOINT)
	{
		if (m_havePatrolWayPoint)
		{
			ClientGameObjectManagerAddon *pGameObjectManagerAddon = (ClientGameObjectManagerAddon *)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
			if (pGameObjectManagerAddon)
			{
				// search for waypoint object
				WayPoint *pWP = pGameObjectManagerAddon->getWayPoint(m_curPatrolWayPoint);
				if (pWP)
				{
					if (pGameObjectManagerAddon->getWayPoint(pWP->m_nextWayPointName))
					{

						pWP = pGameObjectManagerAddon->getWayPoint(pWP->m_nextWayPointName);

					}
					m_state = PATROLLING_WAYPOINTS;
					PE::Handle h("SoldierNPCMovementSM_Event_MOVE_TO", sizeof(SoldierNPCMovementSM_Event_MOVE_TO));
					Events::SoldierNPCMovementSM_Event_MOVE_TO *pEvt = new(h) SoldierNPCMovementSM_Event_MOVE_TO(pWP->m_base.getPos());

					m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
					// release memory now that event is processed
					h.release();
				}
			}
		}
		else
		{
			// should not happen, but in any case, set state to idle
			m_state = IDLE;

			PE::Handle h("SoldierNPCMovementSM_Event_STOP", sizeof(SoldierNPCMovementSM_Event_STOP));
			SoldierNPCMovementSM_Event_STOP* pEvt = new(h) SoldierNPCMovementSM_Event_STOP();

			m_hMovementSM.getObject<Component>()->handleEvent(pEvt);
			// release memory now that event is processed
			h.release();

		}
	}
}


}}




