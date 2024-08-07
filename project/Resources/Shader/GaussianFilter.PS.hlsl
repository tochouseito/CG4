#include"FullScreen.hlsli"

Texture2D<float32_t4> gTexure : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
static const float32_t PI = 3.14159265f;
static const uint32_t size = 5;
static const float powSize = float(size * size);
static const float32_t kKernel[size][size] =
{
    { 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize },
    { 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize },
    { 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize },
    { 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize },
    { 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize, 1.0f / powSize },
};
static const float32_t2 kIndex[size][size] =
{
    { { -1.0f, -1.0f }, { -0.5f, -1.0f }, { 0.0f, -1.0f }, { 0.5f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, -0.5f }, { -0.5f, -0.5f }, { 0.0f, -0.5f }, { 0.5f, -0.5f }, { 1.0f, -0.5f } },
    { { -1.0f, 0.0f }, { -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.5f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 0.5f }, { -0.5f, 0.5f }, { 0.0f, 0.5f }, { 0.5f, 0.5f }, { 1.0f, 0.5f } },
    { { -1.0f, 1.0f }, { -0.5f, 1.0f }, { 0.0f, 1.0f }, { 0.5f, 1.0f }, { 1.0f, 1.0f } },
};
float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    uint32_t width, height; // 1.uvStepSize算出
    float32_t weight = 0.0f;
    float32_t kernel5x5[size][size];
    gTexure.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    for (int32_t x = 0; x < size; ++x)
    {
        for (int32_t y = 0; y < size; ++y)
        {
            kernel5x5[x][y] = gauss(kIndex[x][y].x, kIndex[x][y].y, 2.0f);
            weight += kernel5x5[x][y];
        }
    }
    for (int32_t x = 0; x < size; ++x)
    {
        for (int32_t y = 0; y < size; ++y)
        {
            // 3.現在のtexcoordを算出
            float32_t2 texcoord = input.texcoord + kIndex[x][y] * uvStepSize;
            // 4.色に1/9掛けて足す
            float32_t3 fetchColor = gTexure.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel5x5[x][y];
        }
    }
    output.color.rgb *= rcp(weight);
    return output;
}