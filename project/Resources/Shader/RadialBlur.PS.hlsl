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
    const float32_t2 kCenter = float32_t2(0.5f, 0.5f);// 中心点。ここを基準に放射状にブラーがかかる
    const int32_t kNumSamples = 10;// サンプリング数。多いほど滑らかだが重い
    const float32_t kBlurWidth = 0.01f;// ぼかしの幅。大きいほど大きい
    // 中心から現在のUVに対しての方向を計算
    // 普段方向といえば、単位ベクトルだが、ここでは敢えて正規化せず、遠いほどより遠くをサンプリングする
    float32_t2 direction = input.texcoord - kCenter;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        // 現在のUVからさきほど計算した方向にサンプリング点を進めながらサンプリングしていく
        float32_t2 texcoord = input.texcoord + direction * kBlurWidth * float32_t(sampleIndex);
        // テクスチャ座標をクランプする
        texcoord = clamp(texcoord, float32_t2(0.0f, 0.0f), float32_t2(1.0f, 1.0f));
        outputColor.rgb += gTexure.Sample(gSampler, texcoord).rgb;
    }
    // 平均化する
    outputColor.rgb *= rcp(kNumSamples);
        
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}