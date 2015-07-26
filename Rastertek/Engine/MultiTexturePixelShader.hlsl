////////////////////////////////////////////////////////////////////////////////
// Filename: MultiTexturePixelShader.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture[2];
SamplerState SampleType;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	float4 color1;
	float4 color2;
	float4 blendColor;

	//Get the pixel color from the first texture
	color1 = shaderTexture[0].Sample(SampleType, input.tex);

	//Get the pixel color from the second texture
	color2 = shaderTexture[1].Sample(SampleType, input.tex);

	//Blend the two pixels together and multiply by the gamma value
	blendColor = color1 * color2 * 2.0;

	//Saturate the final color
	blendColor = saturate(blendColor);

	return blendColor;
}