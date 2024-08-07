#include"FullScreen.hlsli"

Texture2D<float32_t4> gTexure : register(t0);
Texture2D<float32_t> gDepthTexture : register(t1);

SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);
struct Material
{
    float32_t4x4 projectionInverse;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
static const uint32_t size = 3;
static const float powSize = float(size * size);
static const float32_t kPrewittHorizontalKernel[size][size] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};
static const float32_t kPrewittVerticalKernel[size][size] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};
static const float32_t2 kIndex[size][size] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};
float32_t Luminance(float32_t3 v)
{
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t2 difference = float32_t2(0.0f, 0.0f); // 縦横それぞれの畳み込みの結果を格納する
    uint32_t width, height; // 1.uvStepSize算出
    gTexure.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    float32_t4x4 pi = gMaterial.projectionInverse;
    // 色を輝度に変換して、畳み込みを行っていく。微分Filter用のkernelになっているので、やること自体は今までの畳み込みと同じ
    for (int32_t x = 0; x < size; ++x)
    {
        for (int32_t y = 0; y < size; ++y)
        {
            float32_t2 texcoord = input.texcoord + kIndex[x][y] * uvStepSize;
            float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
            // NDC->View。P^{-1}においてxとyはzwに影響を与えないので何でもいい。なので、わざわざ行列を渡さなくてもいい
            // gMaterial.projectionInverseはCBufferを使って渡して置くこと
            float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), pi);
            float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
            difference.x += viewZ * kPrewittHorizontalKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }
    }
    // 変化の長さをウェイトとして合成。ウェイトの決定方法もいろいろと考えられる。たとえばdifference.xだけを使えば横方向のエッジが検出される
    float32_t weight = length(difference);
    // 差が小さすぎて分かりづらいので適当に倍している。CBufferで調整パラメータとして送ったりするといい
    weight = saturate(weight);
    // weightが大きいほど暗く表示するようにしている。最もシンプルな合成方法
    output.color.rgb = (1.0f - weight) * gTexure.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    return output;
}