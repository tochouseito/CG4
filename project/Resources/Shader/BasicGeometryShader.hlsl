#include"Primitive.hlsli"
static const uint vnum = 4;
// センターからのオフセット
static const float4 offset_array[vnum] =
{
    float4(-0.5f, -0.5f, 0, 0),
    float4(-0.5f, +0.5f, 0, 0),
    float4(+0.5f, -0.5f, 0, 0),
    float4(+0.5f, +0.5f, 0, 0),
};
static const float2 uv_array[vnum] =
{
    float2(0, 1),
    float2(0, 0),
    float2(1, 1),
    float2(1, 0)
};

[maxvertexcount(vnum)]
void main(
	point VertexShaderOutput input[1] , 
	inout TriangleStream< GSOutput > output
)
{
    GSOutput element;
    for (uint i = 0; i < vnum; i++)
    {
        GSOutput element;
        element.position = input[0].position+offset_array[i];
        element.normal = input[i].normal;
        element.texcoord = uv_array[i];
        
        output.Append(element);
    }
    
    //element.position = input[0].position;
    //element.normal = input[0].normal;
    //element.texcoord = input[0].texcoord;
    //output.Append(element);
    //element.position = input[0].position + float4(10.0f, 10.0f, 0.0f, 0.0f);
    //output.Append(element);
    //element.position = input[0].position + float4(10.0f, 0.0f, 0.0f, 0.0f);;
    //output.Append(element);
    
}