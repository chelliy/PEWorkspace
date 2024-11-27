#ifndef _PE_SOLDIER_NPC_ANIMATION_SM_H_
#define _PE_SOLDIER_NPC_ANIMATION_SM_H_


#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"


#include "../Events/Events.h"

namespace CharacterControl{

// events that can be sent to this state machine
namespace Events
{

// sent by movement state machine when a soldier has to stop
struct SoldierNPCAnimSM_Event_STOP : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_STOP);

	SoldierNPCAnimSM_Event_STOP() {}
};

// sent by movement state machine when a soldier has to walk
struct SoldierNPCAnimSM_Event_WALK : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_WALK);

	SoldierNPCAnimSM_Event_WALK() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_RUN : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_RUN);

	SoldierNPCAnimSM_Event_RUN() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_TAKECOVER : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_TAKECOVER);

	SoldierNPCAnimSM_Event_TAKECOVER() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_JUMPCOVER : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_JUMPCOVER);

	SoldierNPCAnimSM_Event_JUMPCOVER() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_FLOATING : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_FLOATING);

	SoldierNPCAnimSM_Event_FLOATING() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_LANDING : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_LANDING);

	SoldierNPCAnimSM_Event_LANDING() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_HIGHCOVER : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_HIGHCOVER);

	SoldierNPCAnimSM_Event_HIGHCOVER() {}
};

// sent by movement state machine when a soldier has to run
struct SoldierNPCAnimSM_Event_LOWCOVER : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_LOWCOVER);

	SoldierNPCAnimSM_Event_LOWCOVER() {}
};

//// sent by movement state machine when a soldier has to stand shoot
//struct SoldierNPCAnimSM_Event_STAND_SHOOT : public PE::Events::Event {
//	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_STAND_SHOOT);
//
//	SoldierNPCAnimSM_Event_STAND_SHOOT() {}
//};

// todo add events for shooting (sent by weapons state machine

};

namespace Components {

struct SoldierNPCAnimationSM : public PE::Components::DefaultAnimationSM
{
	PE_DECLARE_CLASS(SoldierNPCAnimationSM);
	
	enum AnimId
	{
		NONE = -1,
		STAND = 0,
		WALK = 12,
		RUN = 17,
		TAKECOVER = 9,
		JUMPCOVER = 19,
		HIGHCOVER = 10,
		LOWCOVER = 6,
		FLOATING = 18,
		LANDING = 7,
	};

	SoldierNPCAnimationSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);

	// event handling
	virtual void addDefaultComponents();

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_STOP)
	virtual void do_SoldierNPCAnimSM_Event_STOP(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_WALK)
	virtual void do_SoldierNPCAnimSM_Event_WALK(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_RUN)
	virtual void do_SoldierNPCAnimSM_Event_RUN(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_TAKECOVER)
	virtual void do_SoldierNPCAnimSM_Event_TAKECOVER(PE::Events::Event* pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_JUMPCOVER)
	virtual void do_SoldierNPCAnimSM_Event_JUMPCOVER(PE::Events::Event* pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_FLOATING)
	virtual void do_SoldierNPCAnimSM_Event_FLOATING(PE::Events::Event* pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_LANDING)
	virtual void do_SoldierNPCAnimSM_Event_LANDING(PE::Events::Event* pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_HIGHCOVER)
	virtual void do_SoldierNPCAnimSM_Event_HIGHCOVER(PE::Events::Event* pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_LOWCOVER)
	virtual void do_SoldierNPCAnimSM_Event_LOWCOVER(PE::Events::Event* pEvt);


	AnimId m_curId;
};

};
};


#endif


