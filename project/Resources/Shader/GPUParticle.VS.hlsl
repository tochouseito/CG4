#include"Particle.hlsli"
#include"Mymath.hlsli"

StructuredBuffer<GPUParticle> gParticles : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);
struct VertexShaderInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    GPUParticle particle = gParticles[instanceId];
    float32_t4x4 WVP = mul(gPerView.View, gPerView.Projection);
    float32_t4x4 worldMatrix = MakeAffineMatrix(particle.scale, particle.rotate, particle.translate); // worldMatrixを作る
    
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz *= particle.translate;
    output.position = mul(input.position, mul(worldMatrix, WVP));
    //output.position = mul(input.position, WVP);
    output.texcoord = input.texcoord;
    //output.normal = normalize(mul(input.normal, (float3x3) gParticleWorld[instanceId].World));
    output.color = particle.color;
    return output;
}