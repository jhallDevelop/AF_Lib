#include "ECS/Components/AF_CAI_Behaviour.h"
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CAI_BEHAVIOUR_ZERO
Empty constructor for the AF_CAI_BEHAVIOUR component
====================
*/
AF_CAI_Behaviour AF_CAI_Behaviour_ZERO(void){
	PACKED_CHAR component = AF_FALSE;
	component = AF_Component_SetHas(component, AF_FALSE);
	component = AF_Component_SetEnabled(component, AF_FALSE);
	AF_AI_Action emtyActions = AF_AI_Action_Zero();
	AF_CAI_Behaviour returnAI_Action = {
		//.has = true,
		.enabled = component,
		.actionsArray = {emtyActions, emtyActions, emtyActions, emtyActions, emtyActions, emtyActions, emtyActions, emtyActions},
		.nextAvailableActionSlot = 0

	};
	return returnAI_Action;
}

/*
====================
AF_CAI_BEHAVIOUR_ADD
ADD component and set default values
====================
*/
AF_CAI_Behaviour AF_CAI_Behaviour_ADD(void){//
	PACKED_CHAR component = AF_TRUE;
	component = AF_Component_SetHas(component, AF_TRUE);
	component = AF_Component_SetEnabled(component, AF_TRUE);
	AF_AI_Action emtyActions = AF_AI_Action_Zero();
	AF_CAI_Behaviour returnAI_Action = {
		//.has = true,
		.enabled = component,
		.actionsArray = {emtyActions, emtyActions, emtyActions, emtyActions, emtyActions, emtyActions, emtyActions, emtyActions},
		.nextAvailableActionSlot = 0
	};
	return returnAI_Action;
}

