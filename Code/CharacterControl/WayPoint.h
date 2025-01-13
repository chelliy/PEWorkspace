#ifndef _CHARACTER_CONTROL_WAYPOINT_NPC_
#define _CHARACTER_CONTROL_WAYPOINT_NPC_

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "Events/Events.h"


namespace CharacterControl{
namespace Events{
struct Event_CREATE_WAYPOINT : public PE::Events::Event
{
	PE_DECLARE_CLASS(Event_CREATE_WAYPOINT);

	// override SetLuaFunctions() since we are adding custom Lua interface
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);

	// Lua interface prefixed with l_
	static int l_Construct(lua_State* luaVM);

	Matrix4x4 m_base;
	char m_name[32];
	char m_nextWaypointName[32];

	int m_needToRunToThisWaypoint;

	int m_takingCover;
	int m_highCover;
	int m_jumpStart;
	int m_jumpEnd;
	int m_wallOnRight;
	int m_preStartDistance;

	PEUUID m_peuuid; // unique object id
};
}
namespace Components {

struct WayPoint : public PE::Components::Component
{
	PE_DECLARE_CLASS(WayPoint);

	WayPoint( PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, const Events::Event_CREATE_WAYPOINT *pEvt);

	virtual void addDefaultComponents() ;

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
	void do_PRE_RENDER_needsRC(PE::Events::Event* pEvt);

	char m_name[32];
	char m_nextWayPointName[32];
	Matrix4x4 m_base;
	
	int m_needToRunToThisWaypoint;
	int m_takingCover;
	int m_highCover;
	int m_jumpStart;
	int m_jumpEnd;
	int m_wallOnRight;
	int m_preStartDistance;
};
}; // namespace Components
}; // namespace CharacterControl
#endif

