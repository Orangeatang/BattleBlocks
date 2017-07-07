
// ----------------- Includes -----------------

#include "Stdafx.h"
#include "VETextureManager.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"

#include "DDSTextureLoader.h"


// ----------------- Statics ------------------

int VETextureManager::myNextTextureId = 0;


// --------------- Namespaces -----------------

using namespace DirectX;


// -------------- Class Functions -------------

// Construction
VETextureManager::VETextureManager()
{
}


// Loads a texture, returning a unique id
int VETextureManager::LoadTexture( std::wstring aFileName )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );
		
	VETexture* newTexture = new VETexture();
	std::wstring filePath = VoxelEngine::GetInstance()->GetDataDirectory();
	filePath += L"Textures/";
	filePath += aFileName;

	// Load the texture
	HRESULT result = CreateDDSTextureFromFile( renderInterface->GetDevice(), filePath.c_str(), &newTexture->myResource, &newTexture->myTexture );
	if( FAILED(result) )
	{
		delete newTexture;
		return -1;
	}

	// Create a sampler state for the texture
	result = renderInterface->GetDevice()->CreateSamplerState( &newTexture->mySamplerDescription, &newTexture->mySampler );
	if( FAILED(result) )
	{
		return -1;
	}

	int newId = GetNextTextureId();
	myTextures[newId] = newTexture;

	return newId;
}


// Returns a texture given an ID
VETexture* VETextureManager::GetTexture( int aTextureId )
{
	if( myTextures.find(aTextureId) != myTextures.end() )
	{
		return myTextures[aTextureId];
	}

	return NULL;
}


// Cleans up the memory used by the texture manager
void VETextureManager::Uninitialise()
{
	for( std::map<int, VETexture*>::iterator iter = myTextures.begin(); iter != myTextures.end(); iter++ )
	{
		delete iter->second;
		iter->second = NULL;
	}

	myTextures.clear();
}


// Returns the current texture id, increments the current value
int VETextureManager::GetNextTextureId()
{
	return myNextTextureId++;
}