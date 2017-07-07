#ifndef LOCAL_PLAYER_H
#define LOCAL_PLAYER_H


// --------------------- Includes ---------------------

#include "VEObject.h"


// --------------- Forward Declarations ---------------

class PlayerInputProcessor;


// --------------------- Classes ----------------------

class LocalPlayer : public VEObject
{
	public :

		// ----- Public Functions -----

		// Construction
		LocalPlayer();


		// ---- Required Functions ----

		// Initialises the components used by the local player
		virtual bool Initialise( bool isDynamic = true ) override;


	private :

		// ---- Private Functions ----


		// ---- Private Variables ----

		PlayerInputProcessor* myInputProcessor;
};

#endif // !LOCAL_PLAYER_H