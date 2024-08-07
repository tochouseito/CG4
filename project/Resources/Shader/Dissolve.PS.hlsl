#include"FullScreen.hlsli"

Texture2D<float32_t4> gTexure : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);

SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t threshold = 0.2f;
    float32_t edgeWidth = 0.03f;
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    // maskの値が0.5（閾値）以下の場合はdiscardして抜く
    if (mask <= threshold)
    {
        discard;
    }
    // Edgeっぽさを算出// 0.2は閾値
    float32_t edge = 1.0f - smoothstep(threshold, threshold+edgeWidth, mask);
    output.color = gTexure.Sample(gSampler, input.texcoord);
    // Edgeっぽいほど指定した色を加算
    output.color.rgb += edge * float32_t3(1.0f, 0.4f, 0.3f);
    return output;
}