#ifndef VE_VOXEL_RENDERER_H
#define VE_VOXEL_RENDERER_H


// ------------------ Includes ------------------

#include "VERenderManager.h"


// ------------ Forward Declarations ------------

class VEChunk;
class VEVoxelShader;
class VEBasicCamera;


// ------------------ Classes -------------------

// The VEVoxelRenderManager class is a very basic rendering system, used for drawing voxels on-screen
// without any lighting, shadows or effects. Useful for viewing the scene in wireframe
class VEVoxelRenderManager : public VERenderManager
{
    public :    

        // -------- Public Functions --------

        // Construction
        VEVoxelRenderManager();

        // Initialises the renderer
        virtual bool        Initialise( int aScreenWidth, int aScreenHeight ) override;

        // Cleans up the renderer
        virtual void        Uninitialise() override;

        // Draws all of the voxels
        virtual void        RenderScene() override;


    private :

        // -------- Private Variables -------

        float					myClearColour[4];
		VEDepthStencilTarget*	myDepthStencilTarget;
};


#endif // !VE_VOXEL_RENDERER_H