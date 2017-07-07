///////////////////////////////////////////////////////////
// A vertex shader used to clear deferred rendering buffers
///////////////////////////////////////////////////////////


// ---------------- Structs -----------------

// Vertex shader input
struct VertexShaderInput
{
	float4 myPosition 	: POSITION;
	float2 myTexCoords  : TEXCOORD0;
};

// Pixel shader input
struct PixelShaderInput
{
	float4 myPosition 	: SV_POSITION;
	float2 myTexCoords  : TEXCOORD0;
};


// ----------------- Shader ------------------

// Offsets the position of the vertex by the instanced position
PixelShaderInput VS( VertexShaderInput anInput )
{
	PixelShaderInput output;
	
	anInput.myPosition.w = 1.0f;
	output.myPosition = anInput.myPosition;
	
	output.myTexCoords = anInput.myTexCoords;
	
	return output;
}