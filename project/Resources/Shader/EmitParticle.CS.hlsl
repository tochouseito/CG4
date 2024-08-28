#include"Particle.hlsli"
RWStructuredBuffer<GPUParticle> gParticles : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
[numthreads(1, 1, 1)]
void main( uint32_t DTid : SV_DispatchThreadID )
{
    if (gEmitter.emit != 0)
    {
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            // カウント分Particleを射出する
            gParticles[countIndex] = (GPUParticle) 0;
            gParticles[countIndex].scale = float32_t3(0.3f, 0.3f, 0.3f);
            gParticles[countIndex].translate = float32_t3(0.0f, 0.0f, 0.0f);
            gParticles[countIndex].color = float32_t4(1.0f, 0.0f, 0.0f, 1.0f);

        }

    }

}