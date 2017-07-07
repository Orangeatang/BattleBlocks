#ifndef VE_LIGHT_H
#define VE_LIGHT_H


// --------------------- Includes --------------------

#include "VETypes.h"


// --------------------- Classes ---------------------

// A base light class from which all other light types are derived
class VELight
{
	public :

		// -------- Public Functions --------

		// Construction
		VELight( LightType aLightType ) :
			myLightType( aLightType ),
			myPosition( 0.0f, 0.0f, 0.0f ),
			myColour( 1.0f, 1.0f, 1.0f ),
			myIsDirty( true ),
			myCastsShadows( false )
		{
			XMStoreFloat4x4( &myViewMatrix, DirectX::XMMatrixIdentity() );
			XMStoreFloat4x4( &myProjectionMatrix, DirectX::XMMatrixIdentity() );
		}

		// Deconstruction
		virtual ~VELight()
		{
		}


		// ----------- Accessors ------------

		LightType					GetType()											{ return myLightType; }

		const DirectX::XMFLOAT3&	GetPosition()										{ return myPosition; }
		void						SetPosition( const DirectX::XMFLOAT3& aPosition )	{ myPosition = aPosition; myIsDirty = true; }

		const DirectX::XMFLOAT3&	GetColour()											{ return myColour; }
		void						SetColour( const DirectX::XMFLOAT3& aColour )		{ myColour = aColour; }

		bool						GetCastsShadows()									{ return myCastsShadows; }
		void						SetCastsShadows( bool aCastsShadows )				{ myCastsShadows = aCastsShadows; }

		const DirectX::XMFLOAT4X4&	GetViewMatrix()										
		{
			if( myIsDirty )
			{
				UpdateMatrices();
			}

			return myViewMatrix; 
		}

		const DirectX::XMFLOAT4X4&	GetProjectionMatrix()								
		{
			if( myIsDirty )
			{
				UpdateMatrices();
			}

			return myProjectionMatrix; 
		}


	protected :

		// ------- Protected Functions ------

		// Updates the light's view & projection matrices
		virtual void UpdateMatrices()
		{
			XMStoreFloat4x4( &myProjectionMatrix, DirectX::XMMatrixIdentity() );
			XMStoreFloat4x4( &myViewMatrix, DirectX::XMMatrixIdentity() );
			myIsDirty			= false;
		}


		// ------- Protected Variables ------

		LightType			myLightType;
		DirectX::XMFLOAT3	myPosition;
		DirectX::XMFLOAT3	myColour;

		DirectX::XMFLOAT4X4	myProjectionMatrix;
		DirectX::XMFLOAT4X4	myViewMatrix;

		bool				myIsDirty;
		bool				myCastsShadows;
};

#endif // !VE_LIGHT_H