#ifndef VE_POINT_LIGHT_H
#define VE_POINT_LIGHT_H


// ------------------ Includes ------------------

#include "VELight.h"


// ------------------ Classes -------------------

// Holds information used when rendering point lights
class VEPointLight : public VELight
{
	public :

		// ------ Public Functions ------

		// Construction
		VEPointLight();


		// --------- Accessors ----------

		float	GetRadius()							{ return myRadius; }
		void	SetRadius( float aRadius )			{ myRadius = aRadius; }

		float	GetIntensity()						{ return myIntensity; }
		void	SetIntensity( float anIntensity )	{ myIntensity = anIntensity; }


	private :

		// ------ Private Variables -----

		float	myRadius;
		float	myIntensity;
};


#endif