#include"Sprite.hlsli"

struct WVP
{
    float4x4 View;
    float4x4 Projection;
};
struct World
{
    float4x4 World;
};
ConstantBuffer<WVP> gWVP : register(b0);
ConstantBuffer<World> gWorld : register(b1);

struct SpriteVertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
};
SpriteVertexShaderOutput main(SpriteVertexShaderInput input)
{
    SpriteVertexShaderOutput output;
    //float4x4 WVP = mul(gWVP.View, gWVP.Projection);
    output.position = mul(input.position,gWorld.World);
    output.texcoord = input.texcoord;
    return output;
}