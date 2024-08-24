#include"Skybox.hlsli"
cbuffer Material : register(b0)
{
	float4 colorMaterial : SV_TARGET0;
	int enableLighting;
	float4x4 uvTransform;
	float shininess;
};
struct PixelShaderOutput
{
	float4 colorPS : SV_TARGET0;
};
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutput main(VertexShaderOutput input)
{
	PixelShaderOutput output;
	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	output.colorPS = textureColor * colorMaterial;
	return output;
}