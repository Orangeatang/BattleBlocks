#ifndef VE_DEFERRED_RENDERER_H
#define VE_DEFERRED_RENDERER_H


// ------------------ Forward Declarations -----------------

class VEQuadRenderer;
class VEDirectXInterface;
class VEBasicCamera;
class VELightingManager;
class VEShaderManager;
class VEShader;
class VELight;
class VESphere;


// ------------------------ Includes -----------------------

#include "VERenderManager.h"


// ------------------------ Classes ------------------------

// A deferred renderer implementation. Renders all geometry to a 'g-buffer', then lighting
// is applied as a post processing effect
class VEDeferredRenderManager : public VERenderManager
{
	public :

		// -------- Public Functions --------

		// Construction
		VEDeferredRenderManager();

		// Initialises the render targets used by the deferred renderer
		virtual bool Initialise( int aScreenWidth, int aScreenHeight ) override;

		// Cleans up the memory used by the deferred renderer
		virtual void Uninitialise() override;

		// Uses deferred rendering to draw the scene
		virtual void RenderScene() override;


	private :

		// -------- Private Functions -------

		// Initialises all of the shaders
		bool	InitialiseShaders();

		// Initialises all of the render targets
		bool	InitialiseRenderTargets( VEDirectXInterface* aRenderInterface, int aScreenWidth, int aScreenHeight );

		// Initialises all of the depth stencil states
		bool	InitialiseDepthStencilStates( VEDirectXInterface* aRenderInterface, int aScreenWidth, int aScreenHeight );


		// Clears the depth stencil buffer and the render targets used in the geometry shader
		void	ClearGBuffer( VEDirectXInterface* aRenderInterface, VEShaderManager* aShaderManager );
		
		// Renders all of the chunks to the g-buffer
		void	RenderGBuffer( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VEShaderManager* aShaderManager );


		// Renders an SSAO buffer, used in the final blend shader
		void	RenderSSAO( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, ID3D11RenderTargetView** someRenderTargets, VEShaderManager* aShaderManager );


		// Renders shadows from the light's perspective to a shadow render target
		void	RenderShadowMap( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELight* aLight, VEShaderManager* aShaderManager );
		
		// Renders all of the directional lights to the lighting buffer
		void	RenderDirectionalLights( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELightingManager* aLightingManager, ID3D11RenderTargetView** someRenderTargets, VEShaderManager* aShaderManager );

		// Renders all of the spot lights to the lighting buffer
		void	RenderSpotLights( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELightingManager* aLightingManager, ID3D11RenderTargetView** someRenderTargets, VEShaderManager* aShaderManager );
		
		// Renders all of the point lights to the lighting buffer
		void	RenderPointLights( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VELightingManager* aLightingManager, VEShaderManager* aShaderManager );


		// Blends the lighting buffer with the shadow & colour buffer
		void	RenderFinalFrame( VEDirectXInterface* aRenderInterface, VEBasicCamera* aCamera, VEShaderManager* aShaderManager );


		// Saves the colour, normal and depth buffers to texture files
		void	SaveBuffersToFile( VEDirectXInterface* aRenderInterface );


		// -------- Private Variables -------

		VERenderTarget*				myColourTarget;
		VERenderTarget*				myNormalTarget;
		VERenderTarget*				myDepthRenderTarget;
		VERenderTarget*				mySSAORenderTarget;
		VERenderTarget*				myLightingRenderTarget;
		VERenderTarget*				myShadowRenderTarget;

		VEDepthStencilTarget*		myDepthStencilTarget;
		VEDepthStencilTarget*		myShadowDepthTarget;

		VEQuadRenderer*				myQuadRenderer;

		VESphere*					mySphere;

		int							myRandomNormalsTextureId;

		float						myClearColour[4];
		float						myDepthClearColour[4];
};


#endif // !VE_DEFERRED_RENDERER_H