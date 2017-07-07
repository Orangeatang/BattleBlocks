//////////////////////////////////////////////////////////////////
// A vertex shader that used for shadow maps... sends screen space
// positions through to the pixel shader
//////////////////////////////////////////////////////////////////


// ---------------- Globals ----------------

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
	float2 myTexCoords		: TEXCOORD0;
};

// Pixel shader input
struct PixelShaderInput
{
	float4 myPosition 		: SV_POSITION;
	float4 myDepthPosition  : TEXCOORD0;
};


// ----------------- Shader ------------------

// Sends the screen space position through to the pixel shader
PixelShaderInput VS( VertexShaderInput anInput )
{
	PixelShaderInput output;
	
	// Make sure we have a correct position for the matrix calculations
	anInput.myPosition.w = 1.0f;
	
	// Calculate the screen position of the vertex using the world/view/projection matrices
	output.myPosition = mul( anInput.myPosition, aWorldMatrix );
	output.myPosition = mul( output.myPosition, aViewMatrix );
	output.myPosition = mul( output.myPosition, aProjectionMatrix );
	
	output.myDepthPosition = output.myPosition;
	
	return output;
}