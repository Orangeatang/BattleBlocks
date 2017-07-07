#ifndef VE_SPOT_LIGHT_H
#define VE_SPOT_LIGHT_H


// ---------------------- Includes ----------------------

#include "VELight.h"


// ---------------------- Classes -----------------------

class VESpotLight : public VELight
{
	public :

		// ------- Public Functions -------

		// Construction
		VESpotLight();


		// ---------- Accessors -----------

		// Direction of the light
		const DirectX::XMFLOAT3&	GetDirection()													{ return myLightDirection; myIsDirty = true; }
		void						SetDirection( const DirectX::XMFLOAT3& aLightDirection )		{ myLightDirection = aLightDirection; }

		// Angle through which the spotlight emits light
		float						GetConeAngle()													{ return myConeAngle; }
		void						SetConeAngle( float aConeAngle )								{ myConeAngle = aConeAngle; }

		// Decay rate of the light. Controls how fast the light fades to black from the central point of the spot light to the outer edge
		float						GetDecayRate()													{ return myDecayRate; }
		void						SetDecayRate( float aDecayRate )								{ myDecayRate = aDecayRate; }

		// Radius of the sphere to draw the spot light in
		float						GetRadius()														{ return myRadius; }
		void						SetRadius( float aRadius )										{ myRadius = aRadius; }


	protected :

		// ----- Overridden Functions -----

		// Updates the view/projection matrices for the light
		virtual void				UpdateMatrices() override;


	private :

		// ------- Private Variables ------

		DirectX::XMFLOAT3	myLightDirection;
		float				myConeAngle;
		float				myDecayRate;
		float				myRadius;
};


#endif // !VE_SPOT_LIGHT_H