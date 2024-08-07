#include"Sprite.hlsli"
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
PixelShaderOutput main(SpriteVertexShaderOutput input)
{
    PixelShaderOutput output;
    
    
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    // textureのa値が0のときにPixelを棄却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    if (textureColor.a == 0.0)
    {
        discard;
    }
   
    
        output.colorPS = colorMaterial * textureColor;
    
    return output;
}