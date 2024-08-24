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
struct VertexShaderInput
{
	float32_t4 position : POSITION;
	float32_t2 texcoord : TEXCOORD0;
};
VertexShaderOutput main(VertexShaderInput input){
	VertexShaderOutput output;
	float32_t4x4 WVP = mul(gWVP.View, gWVP.Projection);
	WVP = mul(gWorld[0].World, WVP);
	output.position = mul(input.position, WVP).xyww;
	output.texcoord = input.position.xyz;
	return output;
}