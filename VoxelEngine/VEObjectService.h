#ifndef VE_OBJECT_MANAGER_H
#define VE_OBJECT_MANAGER_H

// --------------------- Includes --------------------

#include "VETypes.h"


// --------------- Forward Declarations --------------

class VEObject;


// --------------------- Classes ---------------------

// A manager used for creating and maintaining engine object instances
class VEObjectService
{
	public :

		// ------ Public Functions ------

		// Construction
		VEObjectService();

		// Deconstruction
		~VEObjectService();

		// Registers an object with the manager
		bool		RegisterObject( VEObject* anObject, bool isDynamic );

		// Returns the object with the supplied id
		VEObject*	GetObject( int anObjectId );

		// Updates all of the registered dynamic objects
		void		Update( float anElapsedTime );


	private :		
		
		// ------ Private Functions ------
		
		// Returns the next object id, increments the counter
		int			GetNextId();


		// ------ Private Variables ------

		std::map<int, VEObject*>	myStaticObjects;
		std::map<int, VEObject*>	myDynamicObjects;
		
		static int					myNextObjectId;
};


#endif // !VE_OBJECT_MANAGER_H