#include "wheel.h"


/**
 *	register a script update routine for engine wheeler
 *	\param routine: update routine and pointer to it's memory
 *	\returns container address of registered routine
 */
lptr<UpdateRoutine> Wheel::call(UpdateRoutine routine)
{
	routines.push_back(routine);
	return std::prev(routines.end());
}

/**
 *	update registered routines
 */
void Wheel::update()
{
	for (UpdateRoutine& p_Routine : routines) p_Routine.update(p_Routine.memory);
}
