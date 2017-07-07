#ifndef VE_RENDERER_H
#define VE_RENDERER_H


// ---------------------- Includes ---------------------

#include "VETypes.h"


// ---------------------- Classes ----------------------

// An abstract class that all render managers in the voxel engine derive from (e.g. VEVoxelRenderManager, VEDeferredRenderManager)
class VERenderManager
{
	public: 

		// --------- Public Functions ---------

		// Construction
		VERenderManager( RenderManagerType aType ) :
			myType( aType ),
			myCaptureFrame( false )
		{
		}


		// Signals that the render manager should save the current frame to images on the disk
		void				CaptureFrame()	{ myCaptureFrame = true; }

		
		// -------- Required Functions --------

		// Initialises the render manager
		virtual bool		Initialise( int aScreenWidth, int aScreenHeight ) = 0;

		// Cleans up the render manager
		virtual void		Uninitialise() = 0;

		// Draws all of the voxels
		virtual void		RenderScene() = 0;


		// ------------ Accessors ------------

		RenderManagerType	GetType()		{ return myType; }


	protected :

		RenderManagerType	myType;
		bool				myCaptureFrame;
};


#endif // !VE_RENDERER_H