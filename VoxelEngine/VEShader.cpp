
// ------------------- Includes -------------------

#include "Stdafx.h"
#include "VEShader.h"

#include "VoxelEngine.h"
#include "VEDirectXInterface.h"
#include "VETypes.h"
#include "VEBasicCamera.h"


// ------------------ Namespaces ------------------

using namespace DirectX;


// ---------------- Class Functions ---------------

// Construction
VEShader::VEShader( int aVertexCBSize, int aPixelCBSize ) :
    myVertexShader( NULL ),
    myPixelShader( NULL ),
    myVertexShaderInputLayout( NULL ),
    myVertexConstantBuffer( NULL ),
    myPixelConstantBuffer( NULL ),
	myShaderResources( NULL ),
	myShaderSamplers( NULL ),
	mySamplerCount( 0 ),
	myResourceCount( 0 ),
	myVertexCBSize( aVertexCBSize ),
	myPixelCBSize( aPixelCBSize )
{
    myPixelShaderFile       = L"";
    myPixelShaderProfile    = "ps_5_0";
    myPixelShaderFunction   = "PS";

    myVertexShaderFunction  = "VS";
    myVertexShaderProfile   = "vs_5_0";
    myVertexShaderFile      = L"";
}


// Deconstruction
VEShader::~VEShader()
{
    if( myVertexShader != NULL )
    {
        myVertexShader->Release();
        myVertexShader = NULL;
    }

    if( myVertexShaderInputLayout != NULL )
    {
        myVertexShaderInputLayout->Release();
        myVertexShaderInputLayout = NULL;
    }

    if( myVertexConstantBuffer != NULL )
    {
        myVertexConstantBuffer->Release();
        myVertexConstantBuffer = NULL;
    }

    if( myPixelShader != NULL ) 
    {
        myPixelShader->Release();
        myPixelShader = NULL;
    }

    if( myPixelConstantBuffer != NULL )
    {
        myPixelConstantBuffer->Release();
        myPixelConstantBuffer = NULL;
    }
}


// Loads the vertex & pixel shaders, initialises the associated buffers
bool VEShader::Initialise()
{
    if( myVertexShaderFile.empty() || myPixelShaderFile.empty() )
    {
        return false;
    }

    // A shader might not contain a vertex shader (post processing effects etc)
    if( !LoadVertexShader() )
    {
        return false;
    }

    // A shader has to contain a pixel shader
    if( !LoadPixelShader() )
    {
        return false;
    }

    // Create the constant & input buffers
    if( myVertexCBSize > 0 && !CreateVertexConstantBuffer() )
    {
        return false;
    }

    // Create the constant and input buffers
    if( myPixelCBSize > 0 && !CreatePixelConstantBuffer() )
    {
        return false;
    }

    return true;
}


// Cleans up the memory used by the shader
void VEShader::Uninitialise()
{
	if( myVertexShader != NULL )
	{
		myVertexShader->Release();
		myVertexShader = NULL;
	}

	if( myVertexConstantBuffer != NULL )
	{
		myVertexConstantBuffer->Release();
		myVertexConstantBuffer = NULL;
	}

	if( myVertexShaderInputLayout != NULL )
	{
		myVertexShaderInputLayout->Release();
		myVertexShaderInputLayout = NULL;
	}

	if( myPixelShader != NULL )
	{
		myPixelShader->Release();
		myPixelShader = NULL;
	}

	if( myPixelConstantBuffer != NULL )
	{
		myPixelConstantBuffer->Release();
		myPixelConstantBuffer = NULL;
	}
}


// Fills the vertex shader constant buffer using the matrices from the supplied camera (world, view, projection)
bool VEShader::PopulateVertexShaderConstants( VEBasicCamera* aCamera, VELight* )
{
	HRESULT						result;
	D3D11_MAPPED_SUBRESOURCE	mappedResource;
	MatrixBuffer*				dataBuffer;
	XMMATRIX					world			= XMLoadFloat4x4( &aCamera->GetWorld() );
	XMMATRIX					view			= XMLoadFloat4x4( &aCamera->GetView() );
	XMMATRIX					projection		= XMLoadFloat4x4( &aCamera->GetProjection() );

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Grab the constant buffer so we can write to it
	result = deviceContext->Map( myVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	if( FAILED(result) )
	{
		return false;
	}

	// Get a pointer to the buffer
	dataBuffer = (MatrixBuffer*)mappedResource.pData;

	// Copy the matrices in to the constant buffer
	dataBuffer->myWorld			= XMMatrixTranspose( world );
	dataBuffer->myView			= XMMatrixTranspose( view );
	dataBuffer->myProjection	= XMMatrixTranspose( projection );

	// Release the constant buffer lock
	deviceContext->Unmap( myVertexConstantBuffer, 0 );

	// Set the constant buffer for the vertex shader
	deviceContext->VSSetConstantBuffers( 0, 1, &myVertexConstantBuffer );

	return true;
}


// We don't have any pixel shader constants for the default shader
bool VEShader::PopulatePixelShaderConstants( VEBasicCamera* aCamera, VELight* aLight )
{
	return true;
}


// Sets the shader resources (textures) for the pixel shader
void VEShader::SetShaderResources( ID3D11ShaderResourceView** someShaderResources, int aShaderResourceCount )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	myShaderResources	= someShaderResources;
	myResourceCount		= aShaderResourceCount;

	renderInterface->GetDeviceContext()->PSSetShaderResources( 0, myResourceCount, myShaderResources );
	
}


// Unbinds the shader resources (textures) from the pixel shader
void VEShader::ClearResources()
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	for( int i = 0; i < myResourceCount; i++ )
	{
		myShaderResources[i] = NULL;
	}

	SetShaderResources( myShaderResources, myResourceCount );
}


// Sets the sampler states used by the pixel shader
void VEShader::SetSamplerStates( ID3D11SamplerState** someSamplerStates, int aSamplerStateCount )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	myShaderSamplers	= someSamplerStates;
	mySamplerCount		= aSamplerStateCount;

	renderInterface->GetDeviceContext()->PSSetSamplers( 0, mySamplerCount, myShaderSamplers );
}


// Unbinds the samplers from the pixel shader
void VEShader::ClearSamplers()
{
	for( int i = 0; i < mySamplerCount; i++ )
	{
		myShaderSamplers[i] = NULL;
	}

	SetSamplerStates( myShaderSamplers, mySamplerCount );
}


// Draws all of the vertices in the input assembler
void VEShader::Draw( int aVertexCount )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Set the input layout for the vertex shader
	deviceContext->IASetInputLayout( myVertexShaderInputLayout );

	// Set the vertex and pixel shaders
	deviceContext->VSSetShader( myVertexShader, NULL, 0 );
	deviceContext->PSSetShader( myPixelShader, NULL, 0 );

	// Draw the vertices
	deviceContext->Draw( aVertexCount, 0 );
}


// Sends the indexed geometry in the input assembler through to the shader
void VEShader::DrawIndexed( int anIndexCount )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Set the input layout for the vertex shader
	deviceContext->IASetInputLayout( myVertexShaderInputLayout );

	// Set the vertex and pixel shaders
	deviceContext->VSSetShader( myVertexShader, NULL, 0 );
	deviceContext->PSSetShader( myPixelShader, NULL, 0 );

	// Draw the vertices
	deviceContext->DrawIndexed( anIndexCount, 0, 0 );
}


// Renders all of the vertices 'anInstanceCount' times, streaming in data from the instance buffer in the input assembler
void VEShader::DrawInstanced( int anIndexCount, int anInstanceCount )
{
	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	ID3D11DeviceContext* deviceContext	= renderInterface->GetDeviceContext();
	assert( deviceContext != NULL );

	// Set the input layout for the vertex shader
	deviceContext->IASetInputLayout( myVertexShaderInputLayout );

	// Set the vertex and pixel shaders
	deviceContext->VSSetShader( myVertexShader, NULL, 0 );
	deviceContext->PSSetShader( myPixelShader, NULL, 0 );

	// Draw the vertices
	deviceContext->DrawIndexedInstanced( anIndexCount, anInstanceCount, 0, 0, 0 );
}


// Logs an error in the shader log file
void VEShader::LogError( ID3D10Blob* anErrorMessage, const std::string& aShaderFile )
{
    std::string     errors;
    unsigned long   bufferSize;
    std::ofstream   outputFile;

    // Grab the errors from the message
	if( anErrorMessage != NULL )
	{
		errors      = (char*)anErrorMessage->GetBufferPointer();
		bufferSize  = anErrorMessage->GetBufferSize();

		anErrorMessage->Release();
		anErrorMessage = NULL;
	}
	else
	{
		errors = "Unable to locate shader file : " + aShaderFile;
	}

    // Write out the errors to a file
    outputFile.open( "shader-log.txt" );
	outputFile.write( errors.c_str(), errors.size() );
    outputFile.close();
}


// Loads and builds the vertex shader
bool VEShader::LoadVertexShader()
{
    VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
    assert( renderInterface != NULL );

    HRESULT     result;
    ID3D10Blob* shaderBuffer = NULL;

    // If the shader is being reloaded, clear the current buffer
    if( myVertexShader != NULL )
    {
        myVertexShader->Release();
        myVertexShader = NULL;
    }

    // Load the vertex shader in to the buffer
    shaderBuffer = LoadShader(myVertexShaderFile, myVertexShaderFunction, myVertexShaderProfile);
    if( shaderBuffer == NULL )
    {
        return false;
    }

    // Build the vertex shader
    result = renderInterface->GetDevice()->CreateVertexShader(
        shaderBuffer->GetBufferPointer(),
        shaderBuffer->GetBufferSize(),
        NULL,
        &myVertexShader
        );
    
    // Create the data layout for the vertex shader
    if( !CreateDataLayout(shaderBuffer) )
    {
        result = -1;
    }

    shaderBuffer->Release();
    shaderBuffer = NULL;

    if( FAILED(result) )
    {
        return false;
    }

    return true;
}


// Loads and builds the pixel shader
bool VEShader::LoadPixelShader()
{
    VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
    assert( renderInterface != NULL );

    HRESULT     result;
    ID3D10Blob* shaderBuffer    = NULL;

    // If the shader is being reloaded, clear the current buffer
    if( myPixelShader != NULL )
    {
        myPixelShader->Release();
        myPixelShader = NULL;
    }

    // Load the pixel shader in to a buffer
    shaderBuffer = LoadShader( myPixelShaderFile, myPixelShaderFunction, myPixelShaderProfile );
    if( shaderBuffer == NULL )
    {
        return false;
    }

    // Build the pixel shader
    result = renderInterface->GetDevice()->CreatePixelShader(
        shaderBuffer->GetBufferPointer(),
        shaderBuffer->GetBufferSize(),
        NULL,
        &myPixelShader
        );
    
    shaderBuffer->Release();
    shaderBuffer = NULL;

    if( FAILED(result) )
    {
        return false;
    }

    return true;
}


// Loads and compiles shader file from disk, storing the result in the supplied buffer
ID3D10Blob* VEShader::LoadShader( const std::wstring& aFileName, const std::string& aMainFunction, const std::string& aShaderProfile )
{
    HRESULT     result;
    ID3D10Blob* errorMessage = NULL;
    ID3D10Blob* shaderBuffer = NULL;

	std::wstring filePath = VoxelEngine::GetInstance()->GetDataDirectory();
	filePath += L"Shaders/";
	filePath += aFileName;

    // Load the shader
    result = D3DCompileFromFile(
			filePath.c_str(),
			NULL,
			NULL,
			aMainFunction.c_str(),
			aShaderProfile.c_str(),
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&shaderBuffer,
			&errorMessage);

    if( FAILED(result) )
    {
        LogError( errorMessage, std::string(aFileName.begin(), aFileName.end()) );
        return NULL;
    }

    return shaderBuffer;
}


// Creates the vertex shader constant buffer (for matrices). In this case it reserves memory for the
// world, view and projection matrices
bool VEShader::CreateVertexConstantBuffer()
{
	HRESULT             result;
	D3D11_BUFFER_DESC   bufferDescription;

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	// Set up the buffer with a byte width of three matrices
	bufferDescription.BindFlags             = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.ByteWidth             = myVertexCBSize;
	bufferDescription.CPUAccessFlags        = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags             = 0;
	bufferDescription.StructureByteStride   = 0;
	bufferDescription.Usage                 = D3D11_USAGE_DYNAMIC;

	// Create the buffer
	result = renderInterface->GetDevice()->CreateBuffer(
		&bufferDescription,
		NULL,
		&myVertexConstantBuffer);

	if( FAILED(result) )
	{
		return false;
	}

	return true;
}


// Creates the pixel shader buffer (lighting parameters)... the default shader doesn't require a constant buffer
// for the pixel shader
bool VEShader::CreatePixelConstantBuffer()
{
	HRESULT             result;
	D3D11_BUFFER_DESC   bufferDescription;

	VEDirectXInterface* renderInterface = VoxelEngine::GetInstance()->GetRenderInterface();
	assert( renderInterface != NULL );

	// Set up the buffer with a byte width of three matrices
	bufferDescription.BindFlags             = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.ByteWidth             = myPixelCBSize;
	bufferDescription.CPUAccessFlags        = D3D11_CPU_ACCESS_WRITE;
	bufferDescription.MiscFlags             = 0;
	bufferDescription.StructureByteStride   = 0;
	bufferDescription.Usage                 = D3D11_USAGE_DYNAMIC;

	// Create the buffer
	result = renderInterface->GetDevice()->CreateBuffer(
		&bufferDescription,
		NULL,
		&myPixelConstantBuffer);

	if( FAILED(result) )
	{
		return false;
	}

	return true;
}