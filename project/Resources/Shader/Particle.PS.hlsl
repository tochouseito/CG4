#include"Particle.hlsli"
cbuffer Material : register(b0)
{
    float4 colorMaterial : SV_TARGET0;
    int enableLighting;
    float4x4 uvTransform;
};
cbuffer DirectionalLight : register(b1)
{
    float4 colorLight; //!<ライトの色
    float3 direction; //!<ライトの向き
    float intensity; //!<輝度
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
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.colorPS = colorMaterial * textureColor * input.color;
    // textureのa値が0のときにPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    if (textureColor.a == 0.0)
    {
        discard;
    }
    if (output.colorPS.a == 0.0)
    {
        discard;
    }
    return output;
}