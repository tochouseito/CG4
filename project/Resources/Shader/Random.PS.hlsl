#include"FullScreen.hlsli"

struct Material
{
    float32_t time;
};

Texture2D<float32_t4> gTexure : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    // 乱数生成。引数にtexcoordに渡している
    float32_t random = rand2dTo1d(input.texcoord * gMaterial.time);
    // 色にする
    output.color = gTexure.Sample(gSampler, input.texcoord);
    output.color.rgb = float32_t3(mul(output.color.rgb, random));
    output.color.a = 1.0f;
    return output;
}