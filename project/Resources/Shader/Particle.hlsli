struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};
struct GPUParticle
{
    float32_t3 translate;
    float32_t3 rotate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};
struct PerView
{
    float32_t4x4 View;
    float32_t4x4 Projection;
    //float32_t4x4 billboardMatrix;
    float32_t3 cameraPosition;
};
struct PerFrame
{
    float32_t time;
    float32_t deltaTime;
};
struct EmitterSphere
{
    float32_t3 translate; 
    float32_t radius;
    uint32_t count; 
    float32_t frequency;
    float32_t frequencyTime; 
    uint32_t emit;
};