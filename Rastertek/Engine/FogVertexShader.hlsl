////////////////////////////////////////////////////////////////////////////////
// Filename: FogVertexShader.hlsl
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer FogBuffer
{
	float fogStart;
	float fogEnd;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float fogFactor : FOG;
};


PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	float4 cameraPosition;
	float fogDensity = 0.12f;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	//Calculate the camera position
	cameraPosition = mul(input.position, worldMatrix);
	cameraPosition = mul(cameraPosition, viewMatrix);

	//Calculate linear fog
	
	output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart)); //Linear Fog
	//output.fogFactor = saturate(pow(0.36787968862663154641905911090837f, cameraPosition.z * fogDensity)); //Exponential Fog
	//output.fogFactor = saturate(pow(0.36787968862663154641905911090837f, ((cameraPosition.z *fogDensity) * cameraPosition.z * fogDensity)));

	return output;
}