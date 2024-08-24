#include"Skybox.hlsli"
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
ConstantBuffer<World> gWorld : register(b1);
struct VertexShaderInput
{
	float4 position : POSITIONT;
	float2 texcoord : TEXCOORD0;
};
VertexShaderOutput main(VertexShaderInput input){
	VertexShaderOutput output;
	float4x4 WVP = mul(gWVP.View, gWVP.Projection);
	WVP = mul(gWorld.World, WVP);
	output.position = mul(input.position, WVP).xyww;
	output.texcoord = input.position.xyz;
}