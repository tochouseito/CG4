#include"FullScreen.hlsli"

Texture2D<float32_t4> gTexure : register(t0);
SamplerState gSampler : register(s0);
struct Material
{
    float32_t hue;
    float32_t saturation;
    float32_t value;
};
ConstantBuffer<Material> gMaterial : register(b0);

float32_t3 RGBToHSV(float32_t3 rgb)
{
    float32_t4 K = float32_t4(0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f);
    float32_t4 p = lerp(float32_t4(rgb.bg, K.wz), float32_t4(rgb.gb, K.xy), step(rgb.b, rgb.g));
    float32_t4 q = lerp(float32_t4(p.xyw, rgb.r), float32_t4(rgb.r, p.yzx), step(p.x, rgb.r));

    float32_t d = q.x - min(q.w, q.y);
    float32_t e = 1.0e-10f;
    return float32_t3(abs(q.z + (q.w - q.y) / (6.0f * d + e)), d / (q.x + e), q.x);
}

float32_t3 HSVToRGB(float32_t3 hsv)
{
    float32_t4 K = float32_t4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
    float32_t3 p = abs(frac(hsv.xxx + K.xyz) * 6.0f - K.www);
    return hsv.z * lerp(K.xxx, saturate(p - K.xxx), hsv.y);
}
float32_t WrapValue(
float32_t value, float32_t minRange, float32_t maxRange)
{
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange, range);
    if (modValue < 0)
    {
        modValue += range;
    }
    return minRange + modValue;
}

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor =
    gTexure.Sample(gSampler, input.texcoord);
    float32_t3 hsv = RGBToHSV(textureColor.rgb);
    
    // ここで調整する
    hsv.x += gMaterial.hue;
    hsv.y += gMaterial.saturation;
    hsv.z += gMaterial.value;
    
    hsv.x = WrapValue(hsv.x, 0.0f, 1.0f);
    hsv.y = saturate(hsv.y);
    hsv.z = saturate(hsv.z);
    
    float32_t3 rgb = HSVToRGB(hsv);
    
    output.color.rgb = rgb;
    output.color.a = textureColor.a;
    
    return output;
}