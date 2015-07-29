////////////////////////////////////////////////////////////////////////////////
// Filename: SpecMapPixelShader.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

Texture2D shaderTextures[3];
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 lightDirection;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	//Sample the texture pixel at this location
	float4 textureColor = shaderTextures[0].Sample(SampleType, input.tex);

	//Sample the pixel in the bump map
	float4 bumpMap = shaderTextures[1].Sample(SampleType, input.tex);

	//Expand the range of the normal value from (0, +1) to (-1, +1).
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	//Calculate the normal from the data in the bump map
	float3 bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;

	//Normalize the resulting bump normal
	bumpNormal = normalize(bumpNormal);

	//Invert the light direction for calculations
	float3 lightDir = -lightDirection;

	//Calculate the amount of light on this pixel based on the bump map normal value
	float lightIntensity = saturate(dot(bumpNormal, lightDir));

	//Determine the final diffuse color based on the diffuse color and the amount of light intensity
	float4 color = saturate(diffuseColor * lightIntensity);

	//Combine the final bump light color with the texture color
	color *= textureColor;

	if (lightIntensity > 0.0f)
	{
		//Sample the pixel from the specular map texture.
		float4 specularIntensity = shaderTextures[2].Sample(SampleType, input.tex);

		//Calculate the reflection vector based on the light intensity, normal vector and light direction
		float3 reflection = normalize(2 * lightIntensity * bumpNormal - lightDir);

		//Determine the amount of specular light based on the refleciton vector, viewing direciton and specular power
		float4 specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		//Use the specular map to determine the intensity of specular light at this pixel
		specular = specular * specularIntensity;

		//Add the specular component last to the output color
		color = saturate(color + specular);
	}

	return color;
}