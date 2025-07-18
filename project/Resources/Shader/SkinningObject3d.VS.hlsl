#include"Obj3d.hlsli"
struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};
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
StructuredBuffer<Well> gMatrixPalette : register(t1);
//ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITIONT;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float32_t4 weight : WEIGHT0;
    int32_t4 index : INDEX0;
};
struct Skinned
{
    float32_t4 position;
    float32_t3 normal;
};
Skinned Skinning(VertexShaderInput input)
{
    Skinned skinned;
    // 位置の変換
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;
    // 法線の変換
    skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal);// 正規化して戻してあげる
    return skinned;
}

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    //Skinned skinned = Skinning(input);
    float4x4 WVP = mul(gWVP.View, gWVP.Projection);
    // RootMatrixの適用
    WVP = mul(gWorld[instanceId].World, WVP);
    //output.position = mul(skinned.position, mul(gWorld[instanceId].rootMatrix, WVP));
    output.position = mul(input.position, mul(gWorld[instanceId].rootMatrix, WVP));
    float4x4 World = mul(gWorld[instanceId].World, gWorld[instanceId].rootMatrix);
    //output.poition = mul(input.poition,gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    
    output.normal = normalize(mul(input.normal, (float3x3) gWorld[instanceId].WorldInverse));
    //output.worldPosition = mul(skinned.position, mul(gWorld[instanceId].World, gWorld[instanceId].rootMatrix)).xyz;
    output.worldPosition = mul(input.position, mul(gWorld[instanceId].World, gWorld[instanceId].rootMatrix)).xyz;
    output.cameraPosition = gWVP.cameraPosition;
    return output;
}