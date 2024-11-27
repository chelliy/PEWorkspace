#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "SoldierNPCAnimationSM.h"
#include "SoldierNPC.h"

using namespace PE::Components;
using namespace PE::Events;

namespace CharacterControl{

namespace Events{
PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_STOP, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_WALK, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_RUN, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_TAKECOVER, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_JUMPCOVER, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_FLOATING, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_LANDING, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_HIGHCOVER, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_LOWCOVER, Event);

//PE_IMPLEMENT_CLASS1(SoldierNPCAnimSM_Event_STAND_SHOOT, Event);

}
namespace Components{

PE_IMPLEMENT_CLASS1(SoldierNPCAnimationSM, DefaultAnimationSM);

SoldierNPCAnimationSM::SoldierNPCAnimationSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : DefaultAnimationSM(context, arena, hMyself)
{
	m_curId = NONE;
}

void SoldierNPCAnimationSM::addDefaultComponents()
{
	DefaultAnimationSM::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_STOP, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_STOP);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_WALK, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_WALK);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_RUN, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_RUN);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_TAKECOVER, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_TAKECOVER);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_JUMPCOVER, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_JUMPCOVER);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_FLOATING, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_FLOATING);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_LANDING, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_LANDING);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_HIGHCOVER, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_HIGHCOVER);
	PE_REGISTER_EVENT_HANDLER(Events::SoldierNPCAnimSM_Event_LOWCOVER, SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_LOWCOVER);
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_STOP(PE::Events::Event *pEvt)
{
	
	if (m_curId != SoldierNPCAnimationSM::STAND)
	{
		m_curId = SoldierNPCAnimationSM::STAND;
		
		setAnimation(0, SoldierNPCAnimationSM::STAND,
		0, 0, 1, 1,
		PE::LOOPING);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_WALK(PE::Events::Event *pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::WALK)
	{
		m_curId = SoldierNPCAnimationSM::WALK;
		setAnimation(0, SoldierNPCAnimationSM::WALK,
			0, 0, 1, 1,
			PE::LOOPING | PE::ROOT_TO_INPLACE);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_RUN(PE::Events::Event *pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::RUN)
	{
		m_curId = SoldierNPCAnimationSM::RUN;
		setAnimation(0, SoldierNPCAnimationSM::RUN,
			0, 0, 1, 1,
			PE::LOOPING | PE::ROOT_TO_INPLACE);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_TAKECOVER(PE::Events::Event* pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::TAKECOVER)
	{
		m_curId = SoldierNPCAnimationSM::TAKECOVER;
		setAnimation(0, SoldierNPCAnimationSM::TAKECOVER,
			0, 0, 1, 1,
			PE::LOOPING | PE::ROOT_TO_INPLACE);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_JUMPCOVER(PE::Events::Event* pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::JUMPCOVER)
	{
		m_curId = SoldierNPCAnimationSM::JUMPCOVER;
		setAnimation(0, SoldierNPCAnimationSM::JUMPCOVER,
			0, 0, 1, 1,
			PE::STAY_ON_ANIMATION_END);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_FLOATING(PE::Events::Event* pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::FLOATING)
	{
		m_curId = SoldierNPCAnimationSM::FLOATING;
		setAnimation(0, SoldierNPCAnimationSM::FLOATING,
			0, 0, 1, 1,
			PE::LOOPING);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_LANDING(PE::Events::Event* pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::LANDING)
	{
		m_curId = SoldierNPCAnimationSM::LANDING;
		setAnimation(0, SoldierNPCAnimationSM::LANDING,
			0, 0, 1, 1,
			PE::ACTIVE);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_HIGHCOVER(PE::Events::Event* pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::HIGHCOVER)
	{
		m_curId = SoldierNPCAnimationSM::HIGHCOVER;
		setAnimation(0, SoldierNPCAnimationSM::HIGHCOVER,
			0, 0, 1, 1,
			PE::LOOPING);
	}
}

void SoldierNPCAnimationSM::do_SoldierNPCAnimSM_Event_LOWCOVER(PE::Events::Event* pEvt)
{
	if (m_curId != SoldierNPCAnimationSM::LOWCOVER)
	{
		m_curId = SoldierNPCAnimationSM::LOWCOVER;
		setAnimation(0, SoldierNPCAnimationSM::LOWCOVER,
			0, 0, 1, 1,
			PE::LOOPING);
	}
}

}}




