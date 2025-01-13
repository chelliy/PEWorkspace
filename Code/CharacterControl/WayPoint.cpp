#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Lua/EventGlue/EventDataCreators.h"


#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "ClientGameObjectManagerAddon.h"
#include "CharacterControlContext.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Render/IRenderer.h"

#include "WayPoint.h"


using namespace PE;
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl{
namespace Events{

PE_IMPLEMENT_CLASS1(Event_CREATE_WAYPOINT, PE::Events::Event);

void Event_CREATE_WAYPOINT::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_CREATE_WAYPOINT[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_CreateSoldierNPC
	luaL_register(luaVM, 0, l_Event_CREATE_WAYPOINT);
}

int Event_CREATE_WAYPOINT::l_Construct(lua_State* luaVM)
{
    PE::Handle h("EVENT", sizeof(Event_CREATE_WAYPOINT));
	Event_CREATE_WAYPOINT *pEvt = new(h) Event_CREATE_WAYPOINT;

	// get arguments from stack
	int numArgs, numArgsConst;
	numArgs = numArgsConst = 22;

	const char* wayPointName = lua_tostring(luaVM, -numArgs--);
	const char* nextWayPointName = lua_tostring(luaVM, -numArgs--);

	int needToRun = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_needToRunToThisWaypoint = needToRun;

	int takeCover = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_takingCover = takeCover;

	int highCover = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_highCover = highCover;

	int jumpStart = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_jumpStart = jumpStart;

	int jumpEnd = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_jumpEnd = jumpEnd;

	int wallOnRight = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_wallOnRight = wallOnRight;

	int preStartDistance = (int)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_preStartDistance = preStartDistance;

	float positionFactor = 1.0f / 100.0f;
	Vector3 pos, u, v, n;
	pos.m_x = (float)lua_tonumber(luaVM, -numArgs--) * positionFactor;
	pos.m_y = (float)lua_tonumber(luaVM, -numArgs--) * positionFactor;
	pos.m_z = (float)lua_tonumber(luaVM, -numArgs--) * positionFactor;

	u.m_x = (float)lua_tonumber(luaVM, -numArgs--); u.m_y = (float)lua_tonumber(luaVM, -numArgs--); u.m_z = (float)lua_tonumber(luaVM, -numArgs--);
	v.m_x = (float)lua_tonumber(luaVM, -numArgs--); v.m_y = (float)lua_tonumber(luaVM, -numArgs--); v.m_z = (float)lua_tonumber(luaVM, -numArgs--);
	n.m_x = (float)lua_tonumber(luaVM, -numArgs--); n.m_y = (float)lua_tonumber(luaVM, -numArgs--); n.m_z = (float)lua_tonumber(luaVM, -numArgs--);

	pEvt->m_peuuid = LuaGlue::readPEUUID(luaVM, -numArgs--);

	

	// set data values before popping memory off stack
	StringOps::writeToString(wayPointName, pEvt->m_name, 32);
	StringOps::writeToString(nextWayPointName, pEvt->m_nextWaypointName, 32);
	
	lua_pop(luaVM, numArgsConst); //Second arg is a count of how many to pop

	pEvt->m_base.loadIdentity();
	pEvt->m_base.setPos(pos);
	pEvt->m_base.setU(u);
	pEvt->m_base.setV(v);
	pEvt->m_base.setN(n);

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}
};
namespace Components {

PE_IMPLEMENT_CLASS1(WayPoint, Component);

// create waypoint form creation event
WayPoint::WayPoint(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself, const Events::Event_CREATE_WAYPOINT *pEvt)
: Component(context, arena, hMyself)
{
	StringOps::writeToString(pEvt->m_name, m_name, 32);
	StringOps::writeToString(pEvt->m_nextWaypointName, m_nextWayPointName, 32);

	m_needToRunToThisWaypoint = pEvt->m_needToRunToThisWaypoint;
	m_takingCover = pEvt->m_takingCover;
	m_highCover = pEvt->m_highCover;
	m_jumpStart = pEvt->m_jumpStart;
	m_jumpEnd = pEvt->m_jumpEnd;
	m_wallOnRight = pEvt->m_wallOnRight;
	m_preStartDistance = pEvt->m_preStartDistance;

	m_base = pEvt->m_base;
}

void WayPoint::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, WayPoint::do_PRE_RENDER_needsRC);
	// custom methods of this component
}


// this event is executed when thread has RC
void WayPoint::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt)
{
	Event_PRE_RENDER_needsRC* pRealEvent = (Event_PRE_RENDER_needsRC*)(pEvt);

	char buf[80];
	sprintf(buf, "Waypoint: %s", m_name);
	DebugRenderer::Instance()->createTextMesh(
		buf, false, false, true, false, 0,
		m_base.getPos() + Vector3(0,5,0), 0.01f, pRealEvent->m_threadOwnershipMask);

	Vector3 color(0, 1.0f, 0);
	Vector3 linepts[] = { m_base.getPos(), color, m_base.getPos() + Vector3(0,4.5,0), color };

	DebugRenderer::Instance()->createLineMesh(false, m_base, &linepts[0].m_x, 2, 0);// send event while the array is on the stack


	{
		//we can also construct points ourself
		bool sent = false;
		ClientGameObjectManagerAddon* pGameObjectManagerAddon = (ClientGameObjectManagerAddon*)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon());
		if (pGameObjectManagerAddon)
		{
			WayPoint* pWP = pGameObjectManagerAddon->getWayPoint(m_nextWayPointName);
			if (pWP)
			{
				if (pWP->m_preStartDistance > 0) {
					//pre start point showcase
					Vector3 target = pWP->m_base.getPos();
					Vector3 pos = m_base.getPos();

					Vector3 dir = pos - target;
					dir.normalize();

					dir *= pWP->m_preStartDistance;

					Vector3 color(1.0f, 0, 1.0f);
					Vector3 linepts[] = { target, color, target + dir, color };

					DebugRenderer::Instance()->createLineMesh(false, m_base, &linepts[0].m_x, 2, 0);// send event while the array is on the stack


					char buf[80];
					if (pWP->m_takingCover) {
						if (pWP->m_wallOnRight) {
							sprintf(buf, "Prestart: Taking Cover Walk when Wall on Right");
						}else{
							sprintf(buf, "Prestart: Taking Cover Walk when Wall on Left");
						}
					}
					if (pWP->m_jumpEnd) {
						sprintf(buf, "Prestart: Jump");
					}
					DebugRenderer::Instance()->createTextMesh(
						buf, false, false, true, false, 0,
						target + dir + Vector3(0, 4, 0), 0.01f, pRealEvent->m_threadOwnershipMask);

					color = Vector3(1.0f, 0, 0);
					Vector3 newlinepts[] = { target + dir, color, target + dir + Vector3(0, 3.5, 0), color };

					DebugRenderer::Instance()->createLineMesh(false, m_base, &newlinepts[0].m_x, 2, 0);// send event while the array is on the stack
				}
				sent = true;
			}

			if (m_jumpStart) {

				char buf[80];
				sprintf(buf, "Prestart: Jump");
				
				DebugRenderer::Instance()->createTextMesh(
					buf, false, false, true, false, 0,
					m_base.getPos() + Vector3(0, 4, 0), 0.01f, pRealEvent->m_threadOwnershipMask);

			}
		}
		if (!sent) // if for whatever reason we didnt retrieve waypoint info, send the event with transform only
			DebugRenderer::Instance()->createLineMesh(true, m_base, NULL, 0, 0);// send event while the array is on the stack
	}
}




}; // namespace Components
}; // namespace CharacterControl
