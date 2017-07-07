#ifndef VE_DIRECTIONAL_LIGHT_H
#define VE_DIRECTIONAL_LIGHT_H


// --------------------- Includes --------------------

#include "VELight.h"


// --------------------- Classes ---------------------

// A light with a position, direction and colour
class VEDirectionalLight : public VELight
{
	public :

		// -------- Public Functions --------

		// Construction
		VEDirectionalLight();


		// ----------- Accessors ------------

		const DirectX::XMFLOAT3&	GetDirection()										{ return myDirection; }
		void						SetDirection( const DirectX::XMFLOAT3& aDirection ) { myDirection = aDirection; myIsDirty = true; }


	protected :

		// ------- Protected Functions ------

		// Creates an orthographic projection matrix for the light
		virtual void				UpdateMatrices() override;



		// ------- Protected Variables ------

		DirectX::XMFLOAT3	myDirection;
};


#endif // !VE_DIRECTIONAL_LIGHT_H