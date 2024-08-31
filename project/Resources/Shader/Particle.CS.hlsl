#include"Particle.hlsli"
static const uint kMaxParticles = 1024;
RWStructuredBuffer<GPUParticle> gParticles : register(u0);
//RWStructuredBuffer<int32_t> gFreeCounter : register(u1);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);
[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // 初期化処理
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        
        gFreeList[particleIndex] = particleIndex;
    }
    if (particleIndex == 0)
    {
        //gFreeCounter[0] = 0;
        gFreeListIndex[0] = kMaxParticles - 1;
    }
}