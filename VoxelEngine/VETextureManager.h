#ifndef VE_TEXTURE_MANAGER_H
#define VE_TEXTURE_MANAGER_H


// ----------------- Includes -----------------

#include "VETypes.h"


// ----------------- Classes ------------------

// Handles loading/unloading of texture resources
class VETextureManager
{
	public :

		// ----- Public Functions -----

		// Construction
		VETextureManager();

		// Loads a texture, returning a unique id
		int			LoadTexture( std::wstring aFileName );

		// Returns a texture given an ID
		VETexture*	GetTexture( int aTextureId );

		// Cleans up the memory used by the texture manager
		void		Uninitialise();


	private :

		// ----- Private Functions ----

		// Returns the current texture id, increments the current value
		int			GetNextTextureId();


		// ----- Private Variables ----

		std::map<int, VETexture*>	myTextures;
		static int					myNextTextureId;
};


#endif // !VE_TEXTURE_MANAGER_H