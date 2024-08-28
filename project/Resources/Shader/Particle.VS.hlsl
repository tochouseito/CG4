#include"Particle.hlsli"


struct ParticleWVP
{
    float4x4 View;
    float4x4 Projection;
    float3 cameraPosition;
};
struct ParticleWorld
{
    float4x4 World;
    float4x4 WorldInverse;
    float4x4 rootMatrix;
};
struct ParticleColor
{
    float4 Color;
};
ConstantBuffer<ParticleWVP> gParticleWVP : register(b0);
StructuredBuffer<ParticleWorld> gParticleWorld : register(t0);
StructuredBuffer<ParticleColor> gParticleColor : register(t1);

struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderInput output;
    float4x4 WVP = mul(gParticleWVP.View,gParticleWVP.Projection);
    output.position = mul(input.position, mul(gParticleWorld[instanceId].World, WVP));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticleWorld[instanceId].World));
    output.color = gParticleColor[instanceId].Color;
    return output;
    
}