
// -------------------------- Includes --------------------------

#include "Stdafx.h"
#include "VELightingManager.h"

#include "VEDirectionalLight.h"
#include "VEPointLight.h"
#include "VESpotLight.h"


// ----------------------- Class Functions ----------------------

// Construction
VELightingManager::VELightingManager() :
	myAmbientColour( 0.8f, 0.8f, 0.8f, 1.0f ),
	myDiffuseColour( 1.0f, 1.0f, 1.0f, 1.0f ),
	myDiffuseLightDirection( 0.0f, -1.0f, 0.0f )
{
}


// Cleans up the memory used by the lighting manager
void VELightingManager::Uninitialise()
{
	// clean up the directional lights
	for( unsigned int i = 0; i < myDirectionalLights.size(); i++ )
	{
		if( myDirectionalLights[i] != NULL )
		{
			delete myDirectionalLights[i];
			myDirectionalLights[i] = NULL;
		}
	}
	myDirectionalLights.clear();

	// clean up the point lights
	for( unsigned int i = 0; i < myPointLights.size(); i++ )
	{
		if( myPointLights[i] != NULL )
		{
			delete myPointLights[i];
			myPointLights[i] = NULL;
		}
	}
	myPointLights.clear();

	// clean up the spot lights
	for( unsigned int i = 0; i < mySpotLights.size(); i++ )
	{
		if( mySpotLights[i] != NULL )
		{
			delete mySpotLights[i];
			mySpotLights[i] = NULL;
		}
	}
}


// Adds a directional light to the lighting manager
VEDirectionalLight* VELightingManager::AddDirectionalLight()
{
	VEDirectionalLight* directonalLight = new VEDirectionalLight();

	myDirectionalLights.push_back( directonalLight );

	return directonalLight;
}


// Adds a point light to the lighting manager
VEPointLight* VELightingManager::AddPointLight()
{
	VEPointLight* pointLight = new VEPointLight();

	myPointLights.push_back( pointLight );

	return pointLight;
}


// Adds a spot light to the lighting manager
VESpotLight* VELightingManager::AddSpotLight()
{
	VESpotLight* spotLight = new VESpotLight();

	mySpotLights.push_back( spotLight );

	return spotLight;
}