
// ------------------ Includes ------------------

#include "Stdafx.h"
#include "VEDirectXInterface.h"


// --------------- Class Functions ---------------

// VEDirectXInterface construction
VEDirectXInterface::VEDirectXInterface() :
	myDevice( NULL ),
	myDeviceContext( NULL ),
	mySwapChain( NULL ),
	myGPUMemory( 0 ),
	myFullscreen( false ),
	myVsync( false ),
	myDefaultRasterState( NULL ),
	myWireframeRasterState( NULL ),
	myFrontFaceCullingState( NULL ),
	myRenderTargets( NULL ),
	myRenderTargetCount( 0 ),
	myDepthStencilView( NULL ),
	myBackBufferRenderTarget( NULL ),
	myDepthTestDisabled( NULL ),
	myDepthTestEnabled( NULL ),
	myBlendStateEnabled( NULL ),
	myBlendStateDisabled( NULL ),
	myScreenWidth( 0 ),
	myScreenHeight( 0 )
{
	memset( &myGPUDescription, 0, sizeof(char) * 128 );
}


// Initialises the connection to the DirectX framework
bool VEDirectXInterface::Initialise( HWND aWindowHandle, int aScreenWidth, int aScreenHeight, bool isFullScreen, bool anEnableVsync )
{
	myFullscreen    = isFullScreen;
	myVsync         = anEnableVsync;
	myScreenWidth	= aScreenWidth;
	myScreenHeight	= aScreenHeight;

	unsigned int numerator      = 0;
	unsigned int denominator    = 0; 

	if( !CalculateRefresh(numerator, denominator, aScreenWidth, aScreenHeight) )
	{
		return false;
	}

	if( !CreateDeviceAndSwapChain(numerator, denominator, aScreenWidth, aScreenHeight, aWindowHandle) )
	{
		return false;
	}

	myBackBufferRenderTarget = new VERenderTarget( aScreenWidth, aScreenHeight );
	if( !BuildBackBufferRenderTargets(aScreenWidth, aScreenHeight) )
	{
		return false;
	}

	if( !CreateDefaultRasterStates() )
	{
		return false;
	}

	if( !CreateDefaultDepthStencilStates() )
	{
		return false;
	}

	if( !CreateDefaultAlphaBlendStates() )
	{
		return false;
	}

	SetDrawMode( DM_BackFaceCulling );
	SetViewport( aScreenWidth, aScreenHeight );

	return true;
}


// Cleans up the DirectX connection
void VEDirectXInterface::Uninitialise()
{
	if( mySwapChain && myFullscreen )
	{
		mySwapChain->SetFullscreenState( false, NULL );
	}

	if( myDefaultRasterState != NULL )
	{
		delete myDefaultRasterState;
		myDefaultRasterState = NULL;
	}

	if( myWireframeRasterState != NULL )
	{
		delete myWireframeRasterState;
		myWireframeRasterState = NULL;
	}

	if( myFrontFaceCullingState != NULL )
	{
		delete myFrontFaceCullingState;
		myFrontFaceCullingState = NULL;
	}

	if( myDeviceContext )
	{
		myDeviceContext->Release();
		myDeviceContext = NULL;
	}

	if( myBackBufferRenderTarget != NULL )
	{
		delete myBackBufferRenderTarget;
		myBackBufferRenderTarget = NULL;
	}

	if( myDepthTestDisabled != NULL )
	{
		delete myDepthTestDisabled;
		myDepthTestDisabled = NULL;
	}

	if( myDepthTestEnabled != NULL )
	{
		delete myDepthTestEnabled;
		myDepthTestEnabled = NULL;
	}

	if( myBlendStateEnabled != NULL )
	{
		myBlendStateEnabled->Release();
		myBlendStateEnabled = NULL;
	}

	if( myBlendStateDisabled != NULL )
	{
		myBlendStateDisabled->Release();
		myBlendStateDisabled = NULL;
	}

	if( mySwapChain )
	{
		mySwapChain->Release();
		mySwapChain = NULL;
	}

	if( myDevice )
	{
		myDevice->Release();
		myDevice = NULL;
	}
}


// Clears the back buffer with the supplied colour, and the depth stencil state
void VEDirectXInterface::Clear( float* aClearColour, bool aClearDepth /* = true */ )
{
	assert( aClearColour != NULL );

	// Clear the back buffer render target
	for( int i = 0; i < myRenderTargetCount; i++ )
	{
		myDeviceContext->ClearRenderTargetView( myRenderTargets[i], aClearColour );
	}

	// Clear the depth buffer render target
	if( myDepthStencilView != NULL && aClearDepth )
	{
		myDeviceContext->ClearDepthStencilView( myDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	}
}


// Swaps the back buffer with the front
void VEDirectXInterface::PresentBuffer()
{
	if( myVsync )
	{
		// Lock to refresh rate
		mySwapChain->Present( 1, 0 );
	}
	else
	{
		// Present as fast as possible
		mySwapChain->Present( 0, 0 );
	}
}


// Creates a drawing render target and texture
bool VEDirectXInterface::CreateRenderTarget( VERenderTarget* aRenderTarget, bool aCreateShaderResource /* = false */ )
{
	if( aRenderTarget == NULL )
	{
		assert( false );
		return false;
	}

	HRESULT result;

	// Get the pointer to the back buffer
	aRenderTarget->myRenderTargetTexture = CreateTexture2D( aRenderTarget->myWidth, aRenderTarget->myHeight, aRenderTarget->myFormat, aRenderTarget->myMipMapCount );
	if( aRenderTarget->myRenderTargetTexture == NULL )
	{
		return false;
	}

	// Create the render target from the back buffer pointer
	result = myDevice->CreateRenderTargetView( aRenderTarget->myRenderTargetTexture, NULL, &aRenderTarget->myRenderTarget );
	if( FAILED(result) )
	{
		return false;
	}

	// If required, create a shader resource and sampler description
	if( aCreateShaderResource )
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		((ID3D11Texture2D *)aRenderTarget->myRenderTargetTexture)->GetDesc(&textureDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;
		ZeroMemory( &shaderResourceDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
		shaderResourceDesc.Format						= textureDesc.Format;
		shaderResourceDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceDesc.Texture2D.MipLevels			= 1;
		shaderResourceDesc.Texture2D.MostDetailedMip	= 0;

		result = myDevice->CreateShaderResourceView( aRenderTarget->myRenderTargetTexture, &shaderResourceDesc, &aRenderTarget->myShaderResource );
		if( FAILED(result) )
		{
			return false;
		}

		result = myDevice->CreateSamplerState( &aRenderTarget->mySamplerDescription, &aRenderTarget->mySamplerState );
		if( FAILED(result) )
		{
			return false;
		}
	}

	return true;
}


// Creates a depth stencil render target and texture
bool VEDirectXInterface::CreateDepthStencilTarget( VEDepthStencilTarget* aDethStencilTarget )
{
	if( aDethStencilTarget == NULL )
	{
		assert( false );
		return false;
	}

	// Create the depth stencil buffer
	if( !BuildDepthStencilBuffer(aDethStencilTarget) )
	{
		return false;
	}

	// Create the depth stencil render target
	if( !BuildDepthStencilRenderTarget(aDethStencilTarget) )
	{
		return false;
	}

	// Create the shader resource
	if( aDethStencilTarget->myShaderResource )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;
		ZeroMemory( &shaderResourceDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
		shaderResourceDesc.Format						= DXGI_FORMAT_R32_FLOAT;
		shaderResourceDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceDesc.Texture2D.MipLevels			= 1;
		shaderResourceDesc.Texture2D.MostDetailedMip	= 0;

		HRESULT result = myDevice->CreateShaderResourceView( aDethStencilTarget->myDepthStencilBuffer, &shaderResourceDesc, &aDethStencilTarget->myDepthShaderResource );
		if( FAILED(result) )
		{
			return false;
		}
	}

	return true;
}


// Creates a depth stencil state
bool VEDirectXInterface::CreateDepthStencilState( VEDepthStencilState* aDepthStencilState )
{
	if( aDepthStencilState == NULL )
	{
		return false;
	}

	HRESULT result;    	

	// Create the depth stencil state
	result = myDevice->CreateDepthStencilState( &aDepthStencilState->myDepthStencilDescription, &aDepthStencilState->myDepthStencilState );
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Creates a raster state
bool VEDirectXInterface::CreateRasterState( VERasterState* aRasterState )
{
	if( aRasterState == NULL )
	{
		return false;
	}

	HRESULT result;

	result = myDevice->CreateRasterizerState( &aRasterState->myRasterDescription, &aRasterState->myRasterState );
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Sets the current render target, to enable the back buffer set the I3D11RenderTargetView to NULL
void VEDirectXInterface::SetRenderTargets( ID3D11RenderTargetView** someRenderTargets, int aRenderTargetCount, ID3D11DepthStencilView* aDepthStencilView )
{
	myRenderTargets		= someRenderTargets;
	myDepthStencilView  = aDepthStencilView;

	myRenderTargetCount = 0;
	for( int i = 0; i < aRenderTargetCount; i++ )
	{
		if( someRenderTargets[i] != NULL )
		{
			myRenderTargetCount++;
		}
	}

	myDeviceContext->OMSetRenderTargets( myRenderTargetCount, someRenderTargets, myDepthStencilView );
}


// Unbinds the render targets from the DirectX context
void VEDirectXInterface::ClearRenderTargets()
{
	for( int i = 0; i < myRenderTargetCount; i++ )
	{
		myRenderTargets[i] = NULL;
	}

	myDeviceContext->OMSetRenderTargets( myRenderTargetCount, myRenderTargets, myDepthStencilView );
}


// Sets the current render target to the back buffer
void VEDirectXInterface::SetBackBufferRenderTarget( ID3D11DepthStencilView* aDepthStencilView )
{
	myRenderTargets		= &myBackBufferRenderTarget->myRenderTarget;
	myRenderTargetCount = 1;
	myDepthStencilView  = aDepthStencilView;

	myDeviceContext->OMSetRenderTargets( myRenderTargetCount, myRenderTargets, myDepthStencilView );
}


// Sets the current depth stencil state, use NULL for the first parameter to disable the depth stencil test
void VEDirectXInterface::SetDepthStencilState( VEDepthStencilState* aDepthStencilState, UINT aStencilRef )
{
	if( aDepthStencilState != NULL )
	{
		myDeviceContext->OMSetDepthStencilState( aDepthStencilState->myDepthStencilState, aStencilRef );
	}
	else
	{
		myDeviceContext->OMSetDepthStencilState( NULL, aStencilRef );
	}
}


// Sets the current raster state
void VEDirectXInterface::SetRasterState( VERasterState* aRasterState )
{
	if( aRasterState == NULL )
	{
		return;
	}

	myDeviceContext->RSSetState( aRasterState->myRasterState );
}


// Enables the default raster state (solid, back face culling)
void VEDirectXInterface::SetDrawMode( DrawMode aDrawMode )
{
	switch( aDrawMode )
	{
		case DM_WireFrame :
			SetRasterState( myWireframeRasterState );
			break;

		case DM_FrontFaceCulling :
			SetRasterState( myFrontFaceCullingState );
			break;

		case DM_BackFaceCulling :
		default :
			SetRasterState( myDefaultRasterState );
			break;
	}
}


// Sets the DirectX viewport
void VEDirectXInterface::SetViewport( int aWidth, int aHeight )
{
	D3D11_VIEWPORT viewport;

	// Set up the viewport for rendering
	viewport.Width      = (float)aWidth;
	viewport.Height     = (float)aHeight;
	viewport.MinDepth   = 0.0f;
	viewport.MaxDepth   = 1.0f;
	viewport.TopLeftX   = 0.0f;
	viewport.TopLeftY   = 0.0f;

	// Create the viewport
	myDeviceContext->RSSetViewports( 1, &viewport );
}


// Sets the depth stencil state to the default enabled state
void VEDirectXInterface::EnableDepthTesting()
{
	SetDepthStencilState( myDepthTestEnabled, 0 );
}


// Sets the depth stencil state to the default disabled state
void VEDirectXInterface::DisableDepthTesting()
{
	SetDepthStencilState( myDepthTestDisabled, 0 );
}


// Rendering will use the default alpha blend state
void VEDirectXInterface::EnableAlphaBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	myDeviceContext->OMSetBlendState( myBlendStateEnabled, blendFactor, 0xFFFFFFFF );
}


// Rendering won't use an alpha blend state
void VEDirectXInterface::DisableAlphaBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	myDeviceContext->OMSetBlendState( myBlendStateDisabled, blendFactor, 0xFFFFFFFF );
}


// Calculates the numerator/denominator of the screen's refresh rate. This accounts for refresh rate of different video card & monitor combinations,
// preventing DirectX from accidentally performing a blit instead of buffer swaps
bool VEDirectXInterface::CalculateRefresh( unsigned int& aNumerator, unsigned int& aDenominator, int aScreenWidth, int aScreenHeight )
{
	HRESULT                         result;
	IDXGIFactory*                   factory;
	IDXGIAdapter*                   adapter;
	IDXGIOutput*                    adapterOutput;
	DXGI_MODE_DESC*                 displayModeList;
	DXGI_ADAPTER_DESC               adapterDescription;
	unsigned int                    modes, stringLength;
	int                             error;

	// Create a DirectX graphics factory
	result = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)&factory );
	if( FAILED(result) )
	{
		return false;
	}

	// Create an adapter for the primary video card
	result = factory->EnumAdapters( 0, &adapter );
	if( FAILED(result) )
	{
		return false;
	}

	// Enumerate the primary display
	result = adapter->EnumOutputs( 0, &adapterOutput );
	if( FAILED(result) )
	{
		return false;
	}

	// Get the number of modes that fit the R8G8B8 display format from the adapter
	result = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modes, NULL );
	if( FAILED(result) )
	{
		return false;
	}

	// Create a list to hold all of the possible display modes
	displayModeList = new DXGI_MODE_DESC[modes];
	if( !displayModeList )
	{
		return false;
	}

	// Now populate the display mode list with the appropriate number of modes
	result = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modes, displayModeList );
	if( FAILED(result) )
	{
		return false;
	}

	// Loop through each of the modes, and find the one that matches the screen width and height - once found store the 
	// numerator and denominator of said refresh rate
	for( unsigned int i = 0; i < modes; i++ )
	{
		if( displayModeList[i].Width == (unsigned int)aScreenWidth && displayModeList[i].Height == aScreenHeight )
		{
			aNumerator   = displayModeList[i].RefreshRate.Numerator;
			aDenominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Finally, access the video card information and the amount of available memory
	result = adapter->GetDesc( &adapterDescription );
	if( FAILED(result) )
	{
		return false;
	}

	// Convert the wide character adapter description to a character array
	myGPUMemory = (int)(adapterDescription.DedicatedVideoMemory / 1024 / 1024);
	error = wcstombs_s( &stringLength, myGPUDescription, 128, adapterDescription.Description, 128 );
	if( error != 0 )
	{
		return false;
	}

	// Release all of the memory we used
	delete [] displayModeList;
	displayModeList = NULL;

	adapterOutput->Release();
	adapterOutput = NULL;

	adapter->Release();
	adapter = NULL;

	factory->Release();
	factory = NULL;

	return true;
}


// Creates the swap chain and device for DirectX
bool VEDirectXInterface::CreateDeviceAndSwapChain( unsigned int aNumerator, unsigned int aDenominator, int aScreenWidth, int aScreenHeight, HWND window )
{
	HRESULT                 result;
	D3D_FEATURE_LEVEL       featureLevel;
	DXGI_SWAP_CHAIN_DESC    swapChainDesc;
	ZeroMemory( &swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC) );

	// Set the swap chain to have a single back buffer
	swapChainDesc.BufferCount = 1;

	// Set the width/height of the buffer
	swapChainDesc.BufferDesc.Width  = aScreenWidth;
	swapChainDesc.BufferDesc.Height = aScreenHeight;

	// Use a 32 bit surface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// If vsync is enabled the number of times the swap chain flips the buffers is locked to the refresh rate (say, 60hz). Otherwise
	// it will simply swap them as often as it can
	if( myVsync )
	{
		swapChainDesc.BufferDesc.RefreshRate.Denominator    = aDenominator;
		swapChainDesc.BufferDesc.RefreshRate.Numerator      = aNumerator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Denominator    = 1;
		swapChainDesc.BufferDesc.RefreshRate.Numerator      = 60;
	}

	// Back buffer usage
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the window handle
	swapChainDesc.OutputWindow = window;

	// Turn multi-sampling off
	swapChainDesc.SampleDesc.Count		= 1;
	swapChainDesc.SampleDesc.Quality	= 0;

	// Full screen?
	if( myFullscreen )
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Unspecified scan line ordering and scaling
	swapChainDesc.BufferDesc.ScanlineOrdering   = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling            = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after swapping to the front buffer
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// No advanced flags
	swapChainDesc.Flags = 0;

	// Feature level to dx11
	featureLevel = D3D_FEATURE_LEVEL_11_0;

#ifdef _DEBUG
	int flags = D3D11_CREATE_DEVICE_DEBUG;
#else
	int flags = 0;
#endif

	// Create the swap chain
	result = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		NULL,
		&myDeviceContext
		);

	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Creates the back buffer render target
bool VEDirectXInterface::BuildBackBufferRenderTargets( int aWidth, int aHeight )
{
	if( myBackBufferRenderTarget == NULL )
	{
		assert( false );
		return false;
	}

	HRESULT result = mySwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&myBackBufferRenderTarget->myRenderTargetTexture );
	if( FAILED(result) )
	{
		return false;
	}

	result = myDevice->CreateRenderTargetView( myBackBufferRenderTarget->myRenderTargetTexture, NULL, &myBackBufferRenderTarget->myRenderTarget );
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Creates a 2D texture of the supplied resolution
ID3D11Texture2D* VEDirectXInterface::CreateTexture2D( int aWidth, int aHeight, DXGI_FORMAT aFormat, int aMipCount )
{
	D3D11_TEXTURE2D_DESC	textureDescription;
	ID3D11Texture2D*		newTexture = NULL;

	ZeroMemory( &textureDescription, sizeof(D3D11_TEXTURE2D_DESC) );
	textureDescription.Width				= aWidth;
	textureDescription.Height				= aHeight;
	textureDescription.Format				= aFormat;
	textureDescription.MipLevels			= aMipCount;
	textureDescription.SampleDesc.Count		= 1;
	textureDescription.SampleDesc.Quality	= 0;
	textureDescription.Usage				= D3D11_USAGE_DEFAULT;
	textureDescription.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDescription.CPUAccessFlags		= 0;
	textureDescription.ArraySize			= 1;
	textureDescription.MiscFlags			= 0;

	HRESULT result = myDevice->CreateTexture2D( &textureDescription, NULL, &newTexture );
	if( FAILED(result) )
	{
		return NULL;
	}

	return newTexture;
}


// Creates the depth/stencil buffer
bool VEDirectXInterface::BuildDepthStencilBuffer( VEDepthStencilTarget* aDethStencilTarget )
{
	if( aDethStencilTarget == NULL )
	{
		assert( false );
		return false;
	}

	HRESULT                         result;
	D3D11_TEXTURE2D_DESC            depthBufferDesc;

	// Set up the description of the depth buffer
	memset( &depthBufferDesc, 0, sizeof(D3D11_TEXTURE2D_DESC) );
	depthBufferDesc.Width               = aDethStencilTarget->myWidth;
	depthBufferDesc.Height              = aDethStencilTarget->myHeight;
	depthBufferDesc.MipLevels           = 1;
	depthBufferDesc.ArraySize           = 1;
	depthBufferDesc.Format              = aDethStencilTarget->myBufferFormat;
	depthBufferDesc.SampleDesc.Count    = 1;
	depthBufferDesc.SampleDesc.Quality  = 0;
	depthBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags           = D3D11_BIND_DEPTH_STENCIL; 
	depthBufferDesc.CPUAccessFlags      = 0;
	depthBufferDesc.MiscFlags           = 0;

	if( aDethStencilTarget->myShaderResource )
	{
		depthBufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	// Create the texture for the depth buffer
	result = myDevice->CreateTexture2D( &depthBufferDesc, NULL, &aDethStencilTarget->myDepthStencilBuffer );
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Creates the render target/view for the depth/stencil buffer
bool VEDirectXInterface::BuildDepthStencilRenderTarget( VEDepthStencilTarget* aDethStencilTarget )
{
	if( aDethStencilTarget == NULL )
	{
		assert( false );
		return false;
	}

	HRESULT                         result;
	D3D11_DEPTH_STENCIL_VIEW_DESC   depthStencilViewDesc;

	// Set up the depth stencil view description
	memset( &depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC) );
	depthStencilViewDesc.Format             = aDethStencilTarget->myViewFormat;
	depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	result = myDevice->CreateDepthStencilView(
		aDethStencilTarget->myDepthStencilBuffer,
		&depthStencilViewDesc,
		&aDethStencilTarget->myDepthStencilView
		);

	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// The raster state enables us to control the rasterization, letting us do things like draw back faces and render in wire frame
bool VEDirectXInterface::CreateDefaultRasterStates()
{
	// Set up the default raster state
	myDefaultRasterState = new VERasterState();
	ZeroMemory( &myDefaultRasterState->myRasterDescription, sizeof(D3D11_RASTERIZER_DESC) );
	myDefaultRasterState->myRasterDescription.AntialiasedLineEnable    = false;
	myDefaultRasterState->myRasterDescription.CullMode                 = D3D11_CULL_BACK;
	myDefaultRasterState->myRasterDescription.DepthBias                = 0;
	myDefaultRasterState->myRasterDescription.DepthBiasClamp           = 0.0f;
	myDefaultRasterState->myRasterDescription.DepthClipEnable          = true;
	myDefaultRasterState->myRasterDescription.FillMode                 = D3D11_FILL_SOLID;
	myDefaultRasterState->myRasterDescription.FrontCounterClockwise    = false;
	myDefaultRasterState->myRasterDescription.MultisampleEnable        = false;
	myDefaultRasterState->myRasterDescription.ScissorEnable            = false;
	myDefaultRasterState->myRasterDescription.SlopeScaledDepthBias     = 0.0f;

	// Create the default rasterization state
	bool result = CreateRasterState( myDefaultRasterState );
	if( !result )
	{
		return false;
	}

	// Set up the wireframe raster state
	myWireframeRasterState = new VERasterState();
	ZeroMemory( &myWireframeRasterState->myRasterDescription, sizeof(D3D11_RASTERIZER_DESC) );
	myWireframeRasterState->myRasterDescription.AntialiasedLineEnable    = false;
	myWireframeRasterState->myRasterDescription.CullMode                 = D3D11_CULL_BACK;
	myWireframeRasterState->myRasterDescription.DepthBias                = 0;
	myWireframeRasterState->myRasterDescription.DepthBiasClamp           = 0.0f;
	myWireframeRasterState->myRasterDescription.DepthClipEnable          = true;
	myWireframeRasterState->myRasterDescription.FillMode                 = D3D11_FILL_WIREFRAME;
	myWireframeRasterState->myRasterDescription.FrontCounterClockwise    = false;
	myWireframeRasterState->myRasterDescription.MultisampleEnable        = false;
	myWireframeRasterState->myRasterDescription.ScissorEnable            = false;
	myWireframeRasterState->myRasterDescription.SlopeScaledDepthBias     = 0.0f;

	// Create the wireframe rasterization state
	result = CreateRasterState( myWireframeRasterState );
	if( !result )
	{
		return false;
	}

	// Set up the front face culling raster state
	myFrontFaceCullingState = new VERasterState();
	ZeroMemory( &myFrontFaceCullingState->myRasterDescription, sizeof(D3D11_RASTERIZER_DESC) );
	myFrontFaceCullingState->myRasterDescription.AntialiasedLineEnable    = false;
	myFrontFaceCullingState->myRasterDescription.CullMode                 = D3D11_CULL_FRONT;
	myFrontFaceCullingState->myRasterDescription.DepthBias                = 0;
	myFrontFaceCullingState->myRasterDescription.DepthBiasClamp           = 0.0f;
	myFrontFaceCullingState->myRasterDescription.DepthClipEnable          = true;
	myFrontFaceCullingState->myRasterDescription.FillMode                 = D3D11_FILL_SOLID;
	myFrontFaceCullingState->myRasterDescription.FrontCounterClockwise    = false;
	myFrontFaceCullingState->myRasterDescription.MultisampleEnable        = false;
	myFrontFaceCullingState->myRasterDescription.ScissorEnable            = false;
	myFrontFaceCullingState->myRasterDescription.SlopeScaledDepthBias     = 0.0f;

	result = CreateRasterState( myFrontFaceCullingState );
	if( !result )
	{
		return false;
	}

	return true;
}


// Creates two depth stencil states - one with depth testing enabled, the other with it disabled
bool VEDirectXInterface::CreateDefaultDepthStencilStates()
{
	myDepthTestDisabled = new VEDepthStencilState();
	memset( &myDepthTestDisabled->myDepthStencilDescription, 0, sizeof(D3D11_DEPTH_STENCIL_DESC) );
	myDepthTestDisabled->myDepthStencilDescription.DepthEnable                    = false;
	myDepthTestDisabled->myDepthStencilDescription.DepthWriteMask                 = D3D11_DEPTH_WRITE_MASK_ALL;
	myDepthTestDisabled->myDepthStencilDescription.DepthFunc                      = D3D11_COMPARISON_LESS_EQUAL;
	myDepthTestDisabled->myDepthStencilDescription.StencilEnable                  = false;
	myDepthTestDisabled->myDepthStencilDescription.StencilReadMask                = 0xff;
	myDepthTestDisabled->myDepthStencilDescription.StencilWriteMask               = 0xff;
	myDepthTestDisabled->myDepthStencilDescription.FrontFace.StencilFailOp        = D3D11_STENCIL_OP_KEEP;
	myDepthTestDisabled->myDepthStencilDescription.FrontFace.StencilDepthFailOp   = D3D11_STENCIL_OP_INCR;
	myDepthTestDisabled->myDepthStencilDescription.FrontFace.StencilPassOp        = D3D11_STENCIL_OP_KEEP;
	myDepthTestDisabled->myDepthStencilDescription.FrontFace.StencilFunc          = D3D11_COMPARISON_ALWAYS;
	myDepthTestDisabled->myDepthStencilDescription.BackFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;
	myDepthTestDisabled->myDepthStencilDescription.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_DECR;
	myDepthTestDisabled->myDepthStencilDescription.BackFace.StencilPassOp         = D3D11_STENCIL_OP_KEEP;
	myDepthTestDisabled->myDepthStencilDescription.BackFace.StencilFunc           = D3D11_COMPARISON_ALWAYS;

	if( !CreateDepthStencilState(myDepthTestDisabled) )
	{
		return false;
	}

	myDepthTestEnabled = new VEDepthStencilState();
	myDepthTestEnabled->myDepthStencilDescription.DepthEnable                    = true;
	myDepthTestEnabled->myDepthStencilDescription.DepthWriteMask                 = D3D11_DEPTH_WRITE_MASK_ALL;
	myDepthTestEnabled->myDepthStencilDescription.DepthFunc                      = D3D11_COMPARISON_LESS_EQUAL;
	myDepthTestEnabled->myDepthStencilDescription.StencilEnable                  = false;
	myDepthTestEnabled->myDepthStencilDescription.StencilReadMask                = 0xff;
	myDepthTestEnabled->myDepthStencilDescription.StencilWriteMask               = 0xff;
	myDepthTestEnabled->myDepthStencilDescription.FrontFace.StencilFailOp        = D3D11_STENCIL_OP_KEEP;
	myDepthTestEnabled->myDepthStencilDescription.FrontFace.StencilDepthFailOp   = D3D11_STENCIL_OP_INCR;
	myDepthTestEnabled->myDepthStencilDescription.FrontFace.StencilPassOp        = D3D11_STENCIL_OP_KEEP;
	myDepthTestEnabled->myDepthStencilDescription.FrontFace.StencilFunc          = D3D11_COMPARISON_ALWAYS;
	myDepthTestEnabled->myDepthStencilDescription.BackFace.StencilFailOp         = D3D11_STENCIL_OP_KEEP;
	myDepthTestEnabled->myDepthStencilDescription.BackFace.StencilDepthFailOp    = D3D11_STENCIL_OP_DECR;
	myDepthTestEnabled->myDepthStencilDescription.BackFace.StencilPassOp         = D3D11_STENCIL_OP_KEEP;
	myDepthTestEnabled->myDepthStencilDescription.BackFace.StencilFunc           = D3D11_COMPARISON_ALWAYS;

	if( !CreateDepthStencilState(myDepthTestEnabled) )
	{
		return false;
	}

	return true;
}


// Creates two alpha blend states - one with alpha blending enabled, the other with it disabled
bool VEDirectXInterface::CreateDefaultAlphaBlendStates()
{
	HRESULT				result;
	D3D11_BLEND_DESC	blendStateDescription;
	
	// Create an alpha enabled blend state description.
	ZeroMemory( &blendStateDescription, sizeof(D3D11_BLEND_DESC) );
	blendStateDescription.RenderTarget[0].BlendEnable			= true;
	blendStateDescription.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend				= D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the enabled alpha blend state
	result = myDevice->CreateBlendState( &blendStateDescription, &myBlendStateEnabled );
	if( FAILED(result) )
	{
		return false;
	}

	// Create the disabled blend state
	blendStateDescription.RenderTarget[0].BlendEnable = false;
	result = myDevice->CreateBlendState(&blendStateDescription, &myBlendStateDisabled);
	if( FAILED(result) )
	{
		return false;
	}

	return true;
}