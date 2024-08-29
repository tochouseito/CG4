#include"Particle.hlsli"
#include"Random.hlsli"
class RandomGenerator
{
    float32_t3 seed;
    float32_t3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float32_t Generate1d()
    {
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};
static const uint32_t kMaxParticles = 1024;
RWStructuredBuffer<GPUParticle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

[numthreads(1, 1, 1)]
void main( uint32_t3 DTid : SV_DispatchThreadID )
{
    
    uint32_t particleIndex = DTid.x;
    
    if (particleIndex < kMaxParticles)
    {
        if (gEmitter.emit != 0)
        {
            RandomGenerator generator;
            generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
            // Generate3d呼ぶたびにseedが変わるので結果すべての乱数が変わる
            for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex)
            {
            // カウント分Particleを射出する
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;

            }

        }
    }
}