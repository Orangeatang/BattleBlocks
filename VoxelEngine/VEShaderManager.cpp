
// ----------------- Includes -----------------

#include "Stdafx.h"
#include "VEShaderManager.h"

#include "VEShader.h"
#include "VEClearGBufferShader.h"
#include "VEGeometryShader.h"
#include "VESSAOShader.h"
#include "VEDirectionalLightShader.h"
#include "VESpotLightShader.h"
#include "VEPointLightShader.h"
#include "VEFinalBlendShader.h"
#include "VEShadowMapShader.h"
#include "VEVoxelShader.h"


// -------------- Class Functions -------------

// Construction
VEShaderManager::VEShaderManager()
{
}


// Cleans up the memory used by all of the shaders
void VEShaderManager::Uninitialise()
{
	for( std::map<VEShaderType, VEShader*>::iterator iter = myShaders.begin(); iter != myShaders.end(); iter++ )
	{
		iter->second->Uninitialise();
		
		delete iter->second;
		iter->second = NULL;
	}

	myShaders.clear();
}


// Loads a shader from disk
bool VEShaderManager::LoadShader( VEShaderType aShaderType )
{
	VEShader* newShader = NULL;

	switch( aShaderType )
	{
		case VST_ClearGBuffer :
			newShader = new VEClearGBufferShader();
			break;

		case VST_RenderGBuffer :
			newShader = new VEGeometryShader();
			break;

		case VST_SSAO :
			newShader = new VESSAOShader();
			break;

		case VST_DirectionalLight :
			newShader = new VEDirectionalLightShader();
			break;
		
		case VST_PointLight :
			newShader = new VEPointLightShader();
			break;

		case VST_SpotLight :
			newShader = new VESpotLightShader();
			break;

		case VST_ShadowMap :
			newShader = new VEShadowMapShader();
			break;

		case VST_FinalBlend :
			newShader = new VEFinalBlendShader();
			break;

		case VST_Voxel :
			newShader = new VEVoxelShader();
			break;

		default :
			break;
	}

	if( newShader == NULL )
	{
		return false;
	}

	if( !newShader->Initialise() )
	{
		return false;
	}

	myShaders[aShaderType] = newShader;

	return true;
}


// Returns a shader from the map
VEShader* VEShaderManager::GetShader( VEShaderType aShaderType )
{
	if( myShaders.find(aShaderType) != myShaders.end() )
	{
		return myShaders[aShaderType];
	}

	return NULL;
}