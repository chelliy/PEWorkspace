#ifndef _PE_SOLDIER_NPC_MOVEMENT_SM_H_
#define _PE_SOLDIER_NPC_MOVEMENT_SM_H_


#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Scene/SceneNode.h"

#include "../Events/Events.h"
#include "PrimeEngine/Physics/PhysicsComponent.h"

namespace CharacterControl{
// events that can be sent to this state machine or sent by this state machine (like TARGET_REACHED)
namespace Events
{

// sent by behavior state machine when a soldier has to go somewhere
struct SoldierNPCMovementSM_Event_MOVE_TO : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCMovementSM_Event_MOVE_TO);

	SoldierNPCMovementSM_Event_MOVE_TO(Vector3 targetPos = Vector3());

	Vector3 m_targetPosition;
	bool m_runniung;
	bool m_takeCover;
	bool m_highCover;
	bool m_jumpStart;
	bool m_jumpEnd;
	bool m_wallOnRight;
	int m_preStartDistance = 0;
};

//// sent by behavior state machine when a soldier has to stand shoot
//struct SoldierNPCMovementSM_Event_STAND_SHOOT : public PE::Events::Event {
//	PE_DECLARE_CLASS(SoldierNPCMovementSM_Event_STAND_SHOOT);
//
//	SoldierNPCMovementSM_Event_STAND_SHOOT(Vector3 targetPos = Vector3());
//
//	Vector3 m_targetPosition;
//};

struct SoldierNPCMovementSM_Event_STOP : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCMovementSM_Event_STOP);

	SoldierNPCMovementSM_Event_STOP()
	{}
};

// sent by this state machine to its components. probably to behavior state machine
struct SoldierNPCMovementSM_Event_TARGET_REACHED : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCMovementSM_Event_TARGET_REACHED);

	SoldierNPCMovementSM_Event_TARGET_REACHED()
	{}
	bool m_takingCover = false;
};

// sent by this state machine to its components. probably to behavior state machine
struct SoldierNPCMovementSM_Event_TAKE_COVER : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCMovementSM_Event_TAKE_COVER);

	SoldierNPCMovementSM_Event_TAKE_COVER()
	{}
	bool m_takingCover = false;
};

};
namespace Components {

// movement state machine talks to associated animation state machine
struct SoldierNPCMovementSM : public PE::Components::Component
{
	PE_DECLARE_CLASS(SoldierNPCMovementSM);
	
	enum States
	{
		STANDING,
		RUNNING_TO_TARGET,
		WALKING_TO_TARGET,
		TAKING_COVER,
		IN_AIR,
		//STAND_SHOOT,
	};


	SoldierNPCMovementSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);

	//////////////////////////////////////////////////////////////////////////
	// utility
	//////////////////////////////////////////////////////////////////////////
	PE::Components::SceneNode *getParentsSceneNode();

	//////////////////////////////////////////////////////////////////////////
	// Component API and Event Handlers
	//////////////////////////////////////////////////////////////////////////
	//
	virtual void addDefaultComponents() ;
	//
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCMovementSM_Event_MOVE_TO)
	virtual void do_SoldierNPCMovementSM_Event_MOVE_TO(PE::Events::Event *pEvt);
	//
	//PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCMovementSM_Event_STAND_SHOOT)
	//virtual void do_SoldierNPCMovementSM_Event_STAND_SHOOT(PE::Events::Event *pEvt);
	//
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCMovementSM_Event_TAKE_COVER)
	virtual void do_SoldierNPCMovementSM_Event_TAKE_COVER(PE::Events::Event* pEvt);
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCMovementSM_Event_STOP)
	virtual void do_SoldierNPCMovementSM_Event_STOP(PE::Events::Event *pEvt);
	//
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE)
	virtual void do_UPDATE(PE::Events::Event *pEvt);

	//////////////////////////////////////////////////////////////////////////
	// Member Variables
	//////////////////////////////////////////////////////////////////////////
	PE::Handle m_hAnimationSM;
	//
	// State
	Vector3 m_targetPostion;
	bool preStartActived = false;
	bool m_takeCover = false;
	bool m_highCover = false;
	bool m_jumpStart = false;
	bool m_jumpEnd = false;
	bool m_wallOnRight = false;
	int m_preStartDistance = 0;

	bool m_jumpWaiting = false;
	bool m_jumped = false;
	bool m_landingAnimationPlayNeed = false;
	bool m_floatingAnimationPlayNeed = false;
	float m_timeCounter = 0;
	float m_jumpTimeCounter = 0;
	float m_jumpStartAnimationCounter = 0;
	Vector3 initialVerticalVelocity = Vector3(0, 10, 0);
	Vector3 lastTimeDisplacement = Vector3(0, 0, 0);

	States m_state;
};

};
};


#endif


