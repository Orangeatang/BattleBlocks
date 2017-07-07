//////////////////////////////////////////////////////////////
// A vertex shader that calculates point lighting information
//////////////////////////////////////////////////////////////


// ---------------- Constants ---------------

// A constant buffer storing the required matrices
cbuffer MatrixBuffer
{
	matrix 	aWorldMatrix;
	matrix	aViewMatrix;
	matrix	aProjectionMatrix;	
};


// ---------------- Structs -----------------

// Vertex shader input
struct VertexShaderInput
{
	float4 myPosition 		: POSITION;
};

// Pixel shader input
struct PixelShaderInput
{
	float4 myPosition 		 : SV_POSITION;
	float4 myScreenPosition  : TEXCOORD0;
};


// ----------------- Shader ------------------

// Aligns texture coordinates so that the pixels align to texels
PixelShaderInput VS( VertexShaderInput anInput )
{
	PixelShaderInput output;
	
	anInput.myPosition.w = 1.0f;
	
	// Calculate the screen position of the vertex using the world/view/projection matrices
	output.myPosition = mul( anInput.myPosition, aWorldMatrix );
	output.myPosition = mul( output.myPosition, aViewMatrix );
	output.myPosition = mul( output.myPosition, aProjectionMatrix );
	
	// Send the screen position through to the pixel shader
	output.myScreenPosition = output.myPosition;
	
	return output;
}