#ifndef VE_DIRECTX_INTERFACE_H
#define VE_DIRECTX_INTERFACE_H


// ------------------ Includes ------------------

#include "VETypes.h"


// ------------------ Classes -------------------

// Handles the connection to the DirectX framework
class VEDirectXInterface
{
	public :

		// -------- Public Functions --------

		// Construction
		VEDirectXInterface();

		// Initialises the connection to the DirectX framework
		bool Initialise( HWND aWindowHandle, int aScreenWidth, int aScreenHeight, bool isFullScreen, bool anEnableVsync );

		// Cleans up the DirectX connection
		void Uninitialise(); 
		
		
		// Clears the current render target with the supplied colour
		void Clear( float* aClearColour, bool aClearDepth = true );

		// Swaps the back buffer with the front
		void PresentBuffer();


		// Creates a drawing render target and texture
		bool CreateRenderTarget( VERenderTarget* aRenderTarget, bool aCreateShaderResource = false );

		// Creates a depth stencil render target and texture
		bool CreateDepthStencilTarget( VEDepthStencilTarget* aDethStencilTarget );

		// Creates a depth stencil state
		bool CreateDepthStencilState( VEDepthStencilState* aDepthStencilState );

		// Creates a raster state
		bool CreateRasterState( VERasterState* aRasterState );


		// Sets the current render targets, to enable the back buffer set the I3D11RenderTargetView to null
		void SetRenderTargets( ID3D11RenderTargetView** someRenderTargets, int aRenderTargetCount, ID3D11DepthStencilView* aDepthStencilView );

		// Unbinds the render targets from the DirectX context
		void ClearRenderTargets();

		// Sets the current render target to the back buffer
		void SetBackBufferRenderTarget( ID3D11DepthStencilView* aDepthStencilView );


		// Sets the current depth stencil state, use NULL for the first parameter to disable the depth stencil test
		void SetDepthStencilState( VEDepthStencilState* aDepthStencilState, UINT aStencilRef );

		// Sets the current raster state
		void SetRasterState( VERasterState* aRasterState );

		// Enables the default raster state (solid, back face culling)
		void SetDrawMode( DrawMode aDrawMode );

		// Sets the DirectX viewport
		void SetViewport( int aWidth, int aHeight );


		// Sets the depth stencil state to the default enabled state
		void EnableDepthTesting();

		// Sets the depth stencil state to the default disabled state
		void DisableDepthTesting();


		// Rendering will use the default alpha blend state
		void EnableAlphaBlending();

		// Rendering won't use an alpha blend state
		void DisableAlphaBlending();


		// ---------- Accessors -------------

		IDXGISwapChain*         GetSwapChain()			{ return mySwapChain; }

		ID3D11Device*           GetDevice()				{ return myDevice; }

		ID3D11DeviceContext*    GetDeviceContext()		{ return myDeviceContext; }

		const VERenderTarget*	GetBackBufferTarget()	{ return myBackBufferRenderTarget; }

		int						GetScreenWidth()		{ return myScreenWidth; }

		int						GetScreenHeight()		{ return myScreenHeight; }


	private :

		// -------- Private Functions -------

		// Calculates the numerator/denominator of the screen's refresh rate
		bool				CalculateRefresh( unsigned int& aNumerator, unsigned int& aDenominator, int aScreenWidth, int aScreenHeight );

		// Creates the swap chain and device for DirectX
		bool				CreateDeviceAndSwapChain( unsigned int aNumerator, unsigned int aDenominator, int aScreenWidth, int aScreenHeight, HWND window  );

		// Creates the back buffer render target
		bool				BuildBackBufferRenderTargets( int aWidth, int aHeight );

		// Creates a 2D texture of the supplied resolution
		ID3D11Texture2D*	CreateTexture2D( int aWidth, int aHeight, DXGI_FORMAT aFormat, int aMipCount );


		// Creates the depth/stencil buffer
		bool				BuildDepthStencilBuffer( VEDepthStencilTarget* aDethStencilTarget );

		// Creates the render target/view for the depth/stencil buffer
		bool				BuildDepthStencilRenderTarget( VEDepthStencilTarget* aDethStencilTarget );


		// The raster state enables us to control the rasterization, letting us do things like draw back faces and render in wire frame
		bool				CreateDefaultRasterStates();

		// Creates two depth stencil states - one with depth testing enabled, the other with it disabled
		bool				CreateDefaultDepthStencilStates();

		// Creates two alpha blend states - one with alpha blending enabled, the other with it disabled
		bool				CreateDefaultAlphaBlendStates();


		// -------- Private Variables -------

		IDXGISwapChain*             mySwapChain;
		ID3D11Device*               myDevice;
		ID3D11DeviceContext*        myDeviceContext;
		
		ID3D11BlendState*			myBlendStateEnabled;
		ID3D11BlendState*			myBlendStateDisabled;

		ID3D11RenderTargetView**	myRenderTargets;
		ID3D11DepthStencilView*		myDepthStencilView;
		int							myRenderTargetCount;

		VERenderTarget*				myBackBufferRenderTarget;
		VEDepthStencilState*		myDepthTestEnabled;
		VEDepthStencilState*		myDepthTestDisabled;
		
		VERasterState*				myDefaultRasterState;
		VERasterState*				myWireframeRasterState;
		VERasterState*				myFrontFaceCullingState;

		int							myScreenWidth;
		int							myScreenHeight;

		int                         myGPUMemory;
		char                        myGPUDescription[128];

		bool                        myFullscreen;
		bool                        myVsync;
};


#endif // !VE_DIRECTX_INTERFACE_H