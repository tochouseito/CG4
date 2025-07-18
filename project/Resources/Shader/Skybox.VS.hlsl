#include"Skybox.hlsli"
struct WVP
{
    float32_t4x4 View;
    float32_t4x4 Projection;
    float32_t3 cameraPosition;
};
struct World
{
    float32_t4x4 World;
    float32_t4x4 WorldInverse;
    float32_t4x4 rootMatrix;
};
ConstantBuffer<WVP> gWVP : register(b0);
StructuredBuffer<World> gWorld : register(t0);
float4x4 TranslateMatrix(float3 translate)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        translate.x, translate.y, translate.z, 1
    );
}
struct VertexShaderInput
{
    float32_t4 position : POSITION;
    float32_t2 texcoord : TEXCOORD0;
};
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float32_t4x4 WVP = mul(gWVP.View, gWVP.Projection);
    float4x4 cameraTranslate = TranslateMatrix(gWVP.cameraPosition);
    output.position = mul(input.position, mul(cameraTranslate, WVP)).xyww;
    output.texcoord = input.position.xyz;
    return output;
}