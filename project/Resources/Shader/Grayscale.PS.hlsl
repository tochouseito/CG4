#include"FullScreen.hlsli"

Texture2D<float32_t4> gTexure : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexure.Sample(gSampler, input.texcoord);
    float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
    //output.color.rgb = float32_t3(value, value, value);
    output.color.rgb = value * float32_t3(1.0f, 73.0f / 107.0f, 43.0f / 107.0f);
    return output;
}