#include"Primitive.hlsli"

struct WVP
{
    float4x4 View;
    float4x4 Projection;
    float3 cameraPosition;
};
struct World
{
    float4x4 World;
    float4x4 WorldInverse;
    float4x4 rootMatrix;
};
ConstantBuffer<WVP> gWVP : register(b0);
StructuredBuffer<World> gWorld : register(t0);
//ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    float4x4 WVP = mul(gWVP.View, gWVP.Projection);
    output.position = mul(input.position, mul(gWorld[instanceId].World, WVP));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gWorld[instanceId].World));
    return output;
}