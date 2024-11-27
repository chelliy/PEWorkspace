#ifndef _CHARACTER_CONTROL_VAMPIRE_NPC_
#define _CHARACTER_CONTROL_VAMPIRE_NPC_

#include "PrimeEngine/Events/Component.h"


#include "../Events/Events.h"

namespace CharacterControl {

	namespace Components {

		struct VampireNPC : public PE::Components::Component
		{
			PE_DECLARE_CLASS(VampireNPC);

			VampireNPC(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself, Events::Event_CreateVampireNPC* pEvt);

			virtual void addDefaultComponents();
		};
	}; // namespace Components
}; // namespace CharacterControl
#endif

#pragma once
