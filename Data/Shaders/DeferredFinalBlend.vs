////////////////////////////////////////////////////////////////////
// A vertex shader that is used to blend a deferred lighting buffer
// with the colour buffer
////////////////////////////////////////////////////////////////////


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
	float2 myTexCoords 	: TEXCOORD0;
};


// ----------------- Shader ------------------

// Aligns texture coordinates so that the pixels align to texels
PixelShaderInput VS( VertexShaderInput anInput )
{
	PixelShaderInput output;
	
	output.myPosition = float4( anInput.myPosition.xyz, 1.0f );
	output.myTexCoords = anInput.myTexCoords;
	
	return output;
}
