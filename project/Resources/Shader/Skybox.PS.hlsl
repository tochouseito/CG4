#include"Skybox.hlsli"
cbuffer Material : register(b0)
{
	float32_t4 colorMaterial : SV_TARGET0;
	int enableLighting;
	float32_t4x4 uvTransform;
	float32_t shininess;
};
struct PixelShaderOutput
{
	float32_t4 colorPS : SV_TARGET0;
};
TextureCube<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
PixelShaderOutput main(VertexShaderOutput input)
{
	PixelShaderOutput output;
	float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
	output.colorPS = textureColor * colorMaterial;
	return output;
}