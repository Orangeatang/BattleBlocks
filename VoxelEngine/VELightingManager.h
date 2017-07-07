#ifndef VE_LIGHTING_MANAGER_H
#define VE_LIGHTING_MANAGER_H


// --------------------- Forward Declarations ------------------

class VEDirectionalLight;
class VEPointLight;
class VESpotLight;


// -------------------------- Classes --------------------------

// The lighting manager keeps track of the diffuse & ambient lighting element, as
// well as any lights used during rendering
class VELightingManager
{
	public :

		// --------- Public Functions ---------

		// Construction
		VELightingManager();

		// Cleans up the memory used by the lighting manager
		void									Uninitialise();

		// Adds a directional light to the lighting manager
		VEDirectionalLight*						AddDirectionalLight();

		// Adds a point light to the lighting manager
		VEPointLight*							AddPointLight();

		// Adds a spot light to the lighting manager
		VESpotLight*							AddSpotLight();


		// ------------ Accessors ------------

		const std::vector<VEDirectionalLight*>&	GetDirectionalLights()												{ return myDirectionalLights; }

		const std::vector<VEPointLight*>&		GetPointLights()													{ return myPointLights; }

		const std::vector<VESpotLight*>&		GetSpotLights()														{ return mySpotLights; }

		const DirectX::XMFLOAT4&				GetAmbientColour()													{ return myAmbientColour; }
		void									SetAmbientColour( const DirectX::XMFLOAT4& anAmbientColour )		{ myAmbientColour = anAmbientColour; }

		const DirectX::XMFLOAT4&				GetDiffuseColour()													{ return myDiffuseColour; }
		void									SetDiffuseColour( const DirectX::XMFLOAT4& aDiffuseColour )			{ myDiffuseColour = aDiffuseColour; }

		const DirectX::XMFLOAT3&				GetDiffuseDirection()												{ return myDiffuseLightDirection; }
		void									SetDiffuseDirection( const DirectX::XMFLOAT3& aDiffuseDirection )	{ myDiffuseLightDirection = aDiffuseDirection; }


	private :

		// --------- Private Variables --------

		DirectX::XMFLOAT4					myAmbientColour;
		DirectX::XMFLOAT4					myDiffuseColour;
		DirectX::XMFLOAT3					myDiffuseLightDirection;

		std::vector<VEDirectionalLight*>	myDirectionalLights;
		std::vector<VEPointLight*>			myPointLights;
		std::vector<VESpotLight*>			mySpotLights;
};


#endif // !VE_LIGHTING_MANAGER_H