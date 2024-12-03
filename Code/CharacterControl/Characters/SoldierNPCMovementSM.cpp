#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "SoldierNPCMovementSM.h"
#include "SoldierNPCAnimationSM.h"
#include "SoldierNPC.h"
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl{

// Events sent by behavior state machine (or other high level state machines)
// these are events that specify where a soldier should move
namespace Events{

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_MOVE_TO, Event);

SoldierNPCMovementSM_Event_MOVE_TO::SoldierNPCMovementSM_Event_MOVE_TO(Vector3 targetPos /* = Vector3 */)
: m_targetPosition(targetPos), m_runniung(false), m_takeCover(false), m_highCover(false), m_jumpStart(false), m_jumpEnd(false), m_wallOnRight(false), m_preStartDistance(0)
{ }


PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_STOP, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_TARGET_REACHED, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_TAKE_COVER, Event);


}

namespace Components{

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM, Component);


SoldierNPCMovementSM::SoldierNPCMovementSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) 
: Component(context, arena, hMyself)
, m_state(STANDING)
{}

SceneNode *SoldierNPCMovementSM::getParentsSceneNode()
{
	PE::Handle hParent = getFirstParentByType<Component>();
	if (hParent.isValid())
	{
		// see if parent has scene node component
		return hParent.getObject<Component>()->getFirstComponent<SceneNode>();
		
	}
	return NULL;
}

void SoldierNPCMovementSM::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_MOVE_TO, SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_MOVE_TO);
	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_STOP, SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_STOP);
	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_TAKE_COVER, SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_TAKE_COVER);
	//PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_STAND_SHOOT, SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_STAND_SHOOT);
	
	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, SoldierNPCMovementSM::do_UPDATE);
}

void SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_MOVE_TO(PE::Events::Event* pEvt)
{
	SoldierNPCMovementSM_Event_MOVE_TO* pRealEvt = (SoldierNPCMovementSM_Event_MOVE_TO*)(pEvt);

	// change state of this state machine
	m_targetPostion = pRealEvt->m_targetPosition;
	m_takeCover = pRealEvt->m_takeCover;
	m_highCover = pRealEvt->m_highCover;
	m_jumpStart = pRealEvt->m_jumpStart;
	m_jumpEnd = pRealEvt->m_jumpEnd;
	m_wallOnRight = pRealEvt->m_wallOnRight;
	m_preStartDistance = pRealEvt->m_preStartDistance;

	if (!m_jumpWaiting) {
		OutputDebugStringA("PE: PROGRESS: SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_MOVE_TO(): receuved event, running: ");
		OutputDebugStringA(pRealEvt->m_runniung ? "True\n" : "False\n");
		if (pRealEvt->m_runniung) {
			m_state = RUNNING_TO_TARGET;

			// make sure the animations are playing

			PE::Handle h("SoldierNPCAnimSM_Event_RUN", sizeof(SoldierNPCAnimSM_Event_RUN));
			Events::SoldierNPCAnimSM_Event_RUN* pOutEvt = new(h) SoldierNPCAnimSM_Event_RUN();

			SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
			pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

			// release memory now that event is processed
			h.release();
		}
		else {
			m_state = WALKING_TO_TARGET;

			// make sure the animations are playing

			PE::Handle h("SoldierNPCAnimSM_Event_WALK", sizeof(SoldierNPCAnimSM_Event_WALK));
			Events::SoldierNPCAnimSM_Event_WALK* pOutEvt = new(h) SoldierNPCAnimSM_Event_WALK();

			SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
			pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

			// release memory now that event is processed
			h.release();
		}
	}
	else {
		m_state = IN_AIR;
		m_jumpWaiting = false;
		m_jumped = true;
		m_floatingAnimationPlayNeed = true;
		m_landingAnimationPlayNeed = false;

		PE::Handle h("SoldierNPCAnimSM_Event_JUMPCOVER", sizeof(SoldierNPCAnimSM_Event_JUMPCOVER));
		Events::SoldierNPCAnimSM_Event_JUMPCOVER* pOutEvt = new(h) SoldierNPCAnimSM_Event_JUMPCOVER();

		SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
		pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

		// release memory now that event is processed
		h.release();
	}
	preStartActived = false;
}

void SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_TAKE_COVER(PE::Events::Event *pEvt)
{
	SoldierNPCMovementSM_Event_TAKE_COVER*pRealEvt = (SoldierNPCMovementSM_Event_TAKE_COVER*)(pEvt);

	OutputDebugStringA("PE: PROGRESS: SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_TAKE_COVER(): receuved event");
	//OutputDebugStringA(m_targetPostion ? "True\n" : "False\n");


	m_state = TAKING_COVER;

	// make sure the animations are playing
	if (m_highCover) {
		SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();

		PE::Handle h("SoldierNPCAnimSM_Event_HIGHCOVER", sizeof(SoldierNPCAnimSM_Event_HIGHCOVER));
		Events::SoldierNPCAnimSM_Event_HIGHCOVER* pOutEvt = new(h) SoldierNPCAnimSM_Event_HIGHCOVER();

		pOutEvt->m_wallOnRight = m_wallOnRight;
		pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

		// release memory now that event is processed
		h.release();
	}
	else {
		SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();

		PE::Handle h("SoldierNPCAnimSM_Event_LOWCOVER", sizeof(SoldierNPCAnimSM_Event_LOWCOVER));
		Events::SoldierNPCAnimSM_Event_LOWCOVER* pOutEvt = new(h) SoldierNPCAnimSM_Event_LOWCOVER();

		pOutEvt->m_wallOnRight = m_wallOnRight;
		pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

		// release memory now that event is processed
		h.release();
	}
}

void SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_STOP(PE::Events::Event *pEvt)
{
	Events::SoldierNPCAnimSM_Event_STOP Evt;

	SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
	pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(&Evt);
}

void SoldierNPCMovementSM::do_UPDATE(PE::Events::Event *pEvt)
{
	if (m_state == WALKING_TO_TARGET || m_state == RUNNING_TO_TARGET || m_state == IN_AIR)
	{
		// see if parent has scene node component
		SceneNode *pSceneNode = getParentsSceneNode();
		if (pSceneNode)
		{
			PhysicsComponent* pPhysicsComponent = pSceneNode->m_hPhysicsComponent.getObject<PhysicsComponent>();

			Vector3 curPos = pPhysicsComponent->m_base.getPos();
			Vector3 correction = pPhysicsComponent->m_correction;
			float dsqr = (m_targetPostion - curPos).lengthSqr();

			bool reached = true;
			if (dsqr > 0.01f)
			{
				// not at the spot yet
				Event_UPDATE *pRealEvt = (Event_UPDATE *)(pEvt);
				float speed = (m_state == WALKING_TO_TARGET) ? 1.4f : 3.0f;
				Vector3 gravity = Vector3(0, -10, 0);
				//if (m_jumpEnd || !pPhysicsComponent->m_onGround) {
				//	m_jumpTimeCounter += pRealEvt->m_frameTime;
				//	pPhysicsComponent->m_onGround = false;
				//}

				Vector3 verticalDir(0, 0, 0);

				float allowedDisp = speed * pRealEvt->m_frameTime;

				Vector3 dir = (m_targetPostion - curPos);
				dir.m_y = 0;

				if (m_state == IN_AIR) {
					pPhysicsComponent->m_onGround = false;
				}

				if (!pPhysicsComponent->m_onGround) {
					if (m_jumped) {
						m_jumpTimeCounter += pRealEvt->m_frameTime; 
						m_jumpStartAnimationCounter += pRealEvt->m_frameTime;
						OutputDebugStringA("jump Added\n");
						Vector3 currentDisplacement = initialVerticalVelocity * m_jumpTimeCounter
							+ gravity * m_jumpTimeCounter * m_jumpTimeCounter / 2;
						verticalDir = currentDisplacement - lastTimeDisplacement;
						lastTimeDisplacement = currentDisplacement;

						if (m_floatingAnimationPlayNeed) {
							PE::Handle h("SoldierNPCAnimSM_Event_FLOATING", sizeof(SoldierNPCAnimSM_Event_FLOATING));
							Events::SoldierNPCAnimSM_Event_FLOATING* pOutEvt = new(h) SoldierNPCAnimSM_Event_FLOATING();

							SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
							pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

							// release memory now that event is processed
							h.release();
							m_floatingAnimationPlayNeed = false;
							m_landingAnimationPlayNeed = true;
						}


						if (verticalDir.m_y < 0 && currentDisplacement.m_y <= 2 && m_landingAnimationPlayNeed) {
							PE::Handle h("SoldierNPCAnimSM_Event_LANDING", sizeof(SoldierNPCAnimSM_Event_LANDING));
							Events::SoldierNPCAnimSM_Event_LANDING* pOutEvt = new(h) SoldierNPCAnimSM_Event_LANDING();

							SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
							pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

							// release memory now that event is processed
							h.release();
							m_landingAnimationPlayNeed = false;
							m_state = WALKING_TO_TARGET;
						}
					}
					else
					{
						m_jumpTimeCounter = 0;
						m_jumpStartAnimationCounter = 0;
						lastTimeDisplacement = Vector3(0, 0, 0);
						OutputDebugStringA("Gravity Added\n");
						dir += (gravity);
					}
				}
				else {
					if (m_jumped) {
						m_jumped = false;
						m_state = WALKING_TO_TARGET;

						// make sure the animations are playing

						PE::Handle h("SoldierNPCAnimSM_Event_WALK", sizeof(SoldierNPCAnimSM_Event_WALK));
						Events::SoldierNPCAnimSM_Event_WALK* pOutEvt = new(h) SoldierNPCAnimSM_Event_WALK();

						SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
						pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

						// release memory now that event is processed
						h.release();
					}
					m_jumpTimeCounter = 0;
					m_jumpStartAnimationCounter = 0;
				}
				


				if (dir.length() != 0) {
					dir.normalize();
				}

				float costheta = dir.dotProduct(correction);
				if (costheta != 0) {
					correction = dir.projectionOnVector(correction);
				}

				dir = dir - correction;
				if (dir.lengthSqr() != 0) {
					dir.normalize();
				}

				float dist = sqrt(dsqr);
				if (dist > allowedDisp)
				{
					dist = allowedDisp; // can move up to allowedDisp
					reached = false; // not reaching destination yet
				}



				// instantaneous turn
				pSceneNode->m_base.turnInDirection(dir, 3.1415f);
				pSceneNode->m_base.setPos(curPos + dir * dist + verticalDir);
			}


			if (!preStartActived) {
				//OutputDebugStringA("PE: PROGRESS: SoldierNPCMovementSM anima preActive Check Happened \n");
				if (m_takeCover || m_jumpEnd) {
					//OutputDebugStringA("PE: PROGRESS: SoldierNPCMovementSM anima preActive start handle \n");
					if (sqrt(dsqr) <= m_preStartDistance) 
					{
						//OutputDebugStringA("PE: PROGRESS: SoldierNPCMovementSM anima preActive active! \n");
						if (m_takeCover) {
							PE::Handle h("SoldierNPCAnimSM_Event_TAKECOVER", sizeof(SoldierNPCAnimSM_Event_TAKECOVER));
							Events::SoldierNPCAnimSM_Event_TAKECOVER* pOutEvt = new(h) SoldierNPCAnimSM_Event_TAKECOVER();
							pOutEvt->m_wallOnRight = m_wallOnRight;

							SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
							pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

							// release memory now that event is processed
							h.release();
						}
						else {
							PE::Handle h("SoldierNPCAnimSM_Event_JUMPCOVER", sizeof(SoldierNPCAnimSM_Event_JUMPCOVER));
							Events::SoldierNPCAnimSM_Event_JUMPCOVER* pOutEvt = new(h) SoldierNPCAnimSM_Event_JUMPCOVER();

							SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
							pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);

							// release memory now that event is processed
							h.release();
							m_jumped = true;
							m_floatingAnimationPlayNeed = true;
							m_jumpTimeCounter = 0;
							m_jumpStartAnimationCounter = 0;
							m_state = IN_AIR;
						}
						preStartActived = true;
					}
				}
			}

			if (reached)
			{
				m_state = STANDING;

				if (m_jumpStart) {
					m_jumpWaiting = true;
					m_jumped = false;
				}
				// target has been reached. need to notify all same level state machines (components of parent)
				{
					PE::Handle h("SoldierNPCMovementSM_Event_TARGET_REACHED", sizeof(SoldierNPCMovementSM_Event_TARGET_REACHED));
					Events::SoldierNPCMovementSM_Event_TARGET_REACHED *pOutEvt = new(h) SoldierNPCMovementSM_Event_TARGET_REACHED();
					pOutEvt->m_takingCover = m_takeCover;
					PE::Handle hParent = getFirstParentByType<Component>();
					if (hParent.isValid())
					{
						hParent.getObject<Component>()->handleEvent(pOutEvt);
					}
					
					// release memory now that event is processed
					h.release();
				}
				if (m_state == STANDING)
				{
					// no one has modified our state based on TARGET_REACHED callback
					// this means we are not going anywhere right now
					// so can send event to animation state machine to stop
					{
						Events::SoldierNPCAnimSM_Event_STOP evt;
						
						SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
						pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(&evt);
					}
				}
			}
		}
	}

	if (m_state == TAKING_COVER) {
		// see if parent has scene node component
		SceneNode* pSN = getParentsSceneNode();
		if (pSN)
		{
			Event_UPDATE* pRealEvt = (Event_UPDATE*)(pEvt);
			m_timeCounter += pRealEvt->m_frameTime;

			if (m_timeCounter >= 3) {
				m_state = STANDING;

				// target has been reached. need to notify all same level state machines (components of parent)

				PE::Handle h("SoldierNPCMovementSM_Event_TARGET_REACHED", sizeof(SoldierNPCMovementSM_Event_TARGET_REACHED));
				Events::SoldierNPCMovementSM_Event_TARGET_REACHED* pOutEvt = new(h) SoldierNPCMovementSM_Event_TARGET_REACHED();
				pOutEvt->m_takingCover = false;
				PE::Handle hParent = getFirstParentByType<Component>();
				if (hParent.isValid())
				{
					hParent.getObject<Component>()->handleEvent(pOutEvt);
				}

				// release memory now that event is processed
				h.release();
				m_timeCounter = 0;
			}				

			if (m_state == STANDING)
			{
				// no one has modified our state based on TARGET_REACHED callback
				// this means we are not going anywhere right now
				// so can send event to animation state machine to stop
				{
					Events::SoldierNPCAnimSM_Event_STOP evt;

					SoldierNPC* pSol = getFirstParentByTypePtr<SoldierNPC>();
					pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(&evt);
				}
			}
		}

	}
}

}}




