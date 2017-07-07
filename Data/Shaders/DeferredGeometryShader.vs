/////////////////////////////////////////////////
// A vertex shader that renders voxel chunks
/////////////////////////////////////////////////


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
	float4 myPosition 			: POSITION;
	float3 myNormal 			: NORMAL; 
	float4 myColour				: TEXCOORD0;
};

// Pixel shader input
struct PixelShaderInput
{
	float4 myPosition 	: SV_POSITION;
	float4 myColour 	: TEXCOORD0;
	float3 myNormal		: TEXCOORD1;
	float3 myDepth		: TEXCOORD2;
};


// ----------------- Shader ------------------

// Offsets the position of the vertex by the instanced position
PixelShaderInput VS( VertexShaderInput anInput )
{
	PixelShaderInput output;
	
	// Make sure we have a correct position for the matrix calculations
	anInput.myPosition.w = 1.0f;
	
	// Calculate the screen position of the vertex using the world/view/projection matrices
	output.myPosition = mul( anInput.myPosition, aWorldMatrix );
	output.myPosition = mul( output.myPosition, aViewMatrix );
	output.myPosition = mul( output.myPosition, aProjectionMatrix );
	
	// Set the colour
	output.myColour = anInput.myColour;
	
	// And the normal
	output.myNormal = mul(anInput.myNormal, aWorldMatrix);
	
	// The depth
	output.myDepth.x = output.myPosition.z;
	output.myDepth.y = output.myPosition.w;
	
	return output;
}