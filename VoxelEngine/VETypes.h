#ifndef VE_RENDER_TYPES_H
#define VE_RENDER_TYPES_H


// -------------------- Defines -------------------

#define SHADOW_MAP_SIZE 1024.0f


// ----------------- Enumerations -----------------

// Various types of camera
enum CameraType
{
	CT_Basic,
	CT_FirstPerson,
	CT_Free,

	CT_Max
};


// Draw mode for the render interface
enum DrawMode
{
	DM_BackFaceCulling,
	DM_FrontFaceCulling,
	DM_WireFrame,

	DM_Max
};


// Types of voxels supported
enum VoxelType
{
	VT_Grass,
	VT_Earth,
	VT_Water,
	VT_Sand,
	VT_Stone,
	VT_Wood,

	VT_Max
};


// Types of chunks that can be built
enum ChunkStyle
{
	CS_Box,
	CS_Sphere,
	CS_Pyramid,
	CS_Empty,

	CS_Max
};


// The renderer types supported by the engine
enum RenderManagerType
{
	RT_Voxel,
	RT_Deferred,

	RT_Max
};


// Enumeration for the different types of lights available
enum LightType
{
	LT_Directional,
	LT_Point,
	LT_Spot,

	LT_Max
};


// Enumeration for the different types of object component
enum ComponentType
{
	CM_Physics,

	CM_Max
};


// ------------------ Structures ------------------

// Voxel vertices structure
struct VoxelVertices
{
	// Default constructor
	VoxelVertices()
	{
	}

	// Construction
	VoxelVertices( DirectX::XMFLOAT3 aPosition, DirectX::XMFLOAT3 aNormal, DirectX::XMFLOAT4 aColour )
	{
		myPosition  = aPosition;
		myNormal    = aNormal;
		myColour	= aColour;
	}

	DirectX::XMFLOAT3 myPosition;
	DirectX::XMFLOAT3 myNormal;
	DirectX::XMFLOAT4 myColour;
};


// Default vertex shader constant buffer structure
struct MatrixBuffer
{
	DirectX::XMMATRIX myWorld;
	DirectX::XMMATRIX myView;
	DirectX::XMMATRIX myProjection;
};


// A housing for render target resources
struct VERenderTarget
{
	// Construction
	VERenderTarget( int aWidth, int aHeight, DXGI_FORMAT aFormat = DXGI_FORMAT_D24_UNORM_S8_UINT, int aMipMapCount = 1 ) :
		myRenderTargetTexture( NULL ),
		myRenderTarget( NULL ),
		myShaderResource( NULL ),
		mySamplerState( NULL ),
		myFormat( aFormat ),
		myMipMapCount( aMipMapCount ),
		myWidth( aWidth ),
		myHeight( aHeight )
	{
		// Default sampler description
		ZeroMemory( &mySamplerDescription, sizeof(D3D11_SAMPLER_DESC) );
		mySamplerDescription.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		mySamplerDescription.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
		mySamplerDescription.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
		mySamplerDescription.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
		mySamplerDescription.MipLODBias		= 0.0f;
		mySamplerDescription.MaxAnisotropy	= 1;
		mySamplerDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		mySamplerDescription.BorderColor[0] = 0;
		mySamplerDescription.BorderColor[1] = 0;
		mySamplerDescription.BorderColor[2] = 0;
		mySamplerDescription.BorderColor[3] = 0;
		mySamplerDescription.MinLOD			= 0;
		mySamplerDescription.MaxLOD			= D3D11_FLOAT32_MAX;
	}

	// Deconstruction
	~VERenderTarget()
	{
		if( myRenderTargetTexture != NULL )
		{
			myRenderTargetTexture->Release();
			myRenderTargetTexture = NULL;
		}

		if( myRenderTarget != NULL )
		{
			myRenderTarget->Release();
			myRenderTargetTexture = NULL;
		}

		if( myShaderResource != NULL )
		{
			myShaderResource->Release();
			myShaderResource = NULL;
		}

		if( mySamplerState != NULL )
		{
			mySamplerState->Release();
			mySamplerState = NULL;
		}
	}

	// Resources
	ID3D11RenderTargetView*		myRenderTarget;
	ID3D11Texture2D*			myRenderTargetTexture;
	ID3D11ShaderResourceView*	myShaderResource;
	ID3D11SamplerState*			mySamplerState;

	// Settings
	DXGI_FORMAT					myFormat;
	int							myMipMapCount;
	D3D11_SAMPLER_DESC			mySamplerDescription;

	int							myWidth;
	int							myHeight;
};


// A housing for depth stencil render target resources
struct VEDepthStencilTarget
{
	// Construction
	VEDepthStencilTarget( int aWidth, int aHeight, DXGI_FORMAT aBufferFormat, DXGI_FORMAT aViewFormat, bool aShaderResource = false ) :
		myDepthStencilBuffer( NULL ),
		myDepthStencilView( NULL ),
		myDepthShaderResource( NULL ),
		myWidth( aWidth ),
		myHeight( aHeight ),
		myBufferFormat( aBufferFormat ),
		myViewFormat( aViewFormat ),
		myShaderResource( aShaderResource )
	{
	}

	 // Deconstruction
	~VEDepthStencilTarget()
	{
		if( myDepthStencilBuffer != NULL )
		{
			myDepthStencilBuffer->Release();
			myDepthStencilBuffer = NULL;
		}

		if( myDepthStencilView != NULL )
		{
			myDepthStencilView->Release();
			myDepthStencilView = NULL;
		}

		if( myDepthShaderResource != NULL )
		{
			myDepthShaderResource->Release();
			myDepthShaderResource = NULL;
		}
	}

	// Resources
	ID3D11Texture2D*			myDepthStencilBuffer;
	ID3D11DepthStencilView*     myDepthStencilView;
	ID3D11ShaderResourceView*	myDepthShaderResource;

	// Settings
	DXGI_FORMAT					myBufferFormat;
	DXGI_FORMAT					myViewFormat;
	bool						myShaderResource;

	// Settings
	int							myWidth;
	int							myHeight;
};


// A housing for depth stencil state settings
struct VEDepthStencilState
{
	// Construction
	VEDepthStencilState() :
		myDepthStencilState( NULL )
	{
	}

	// Deconstruction
	~VEDepthStencilState()
	{
		if( myDepthStencilState != NULL )
		{
			myDepthStencilState->Release();
			myDepthStencilState = NULL;
		}
	}
		
	// Resources
	ID3D11DepthStencilState*    myDepthStencilState;
	D3D11_DEPTH_STENCIL_DESC	myDepthStencilDescription;
};


// A housing for raster state settings
struct VERasterState
{
	// Construction
	VERasterState() :
		myRasterState( NULL )
	{

	}

	// Deconstruction
	~VERasterState()
	{
		if( myRasterState != NULL )
		{
			myRasterState->Release();
			myRasterState =  NULL;
		}
	}

	// Resources
	ID3D11RasterizerState*	myRasterState;
	D3D11_RASTERIZER_DESC	myRasterDescription;
};


// A housing for texture data
struct VETexture
{
	// Construction
	VETexture() :
		myResource( NULL ),
		myTexture( NULL ),
		mySampler( NULL )
	{
		ZeroMemory( &mySamplerDescription, sizeof(D3D11_SAMPLER_DESC) );
		mySamplerDescription.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		mySamplerDescription.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
		mySamplerDescription.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
		mySamplerDescription.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
		mySamplerDescription.MipLODBias		= 0.0f;
		mySamplerDescription.MaxAnisotropy	= 1;
		mySamplerDescription.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		mySamplerDescription.BorderColor[0]	= 0;
		mySamplerDescription.BorderColor[1]	= 0;
		mySamplerDescription.BorderColor[2]	= 0;
		mySamplerDescription.BorderColor[3]	= 0;
		mySamplerDescription.MinLOD			= 0;
		mySamplerDescription.MaxLOD			= D3D11_FLOAT32_MAX;
	}

	// Deconstruction
	~VETexture()
	{
		if( myResource != NULL )
		{
			myResource->Release();
			myResource = NULL;
		}

		if( myTexture != NULL )
		{
			myTexture->Release();
			myTexture = NULL;
		}

		if( mySampler != NULL )
		{
			mySampler->Release();
			mySampler = NULL;
		}
	}

	// Resources
	ID3D11Resource*				myResource;
	ID3D11ShaderResourceView*	myTexture;
	ID3D11SamplerState*			mySampler;
	D3D11_SAMPLER_DESC			mySamplerDescription;
};

#endif // !VE_RENDER_TYPES_H