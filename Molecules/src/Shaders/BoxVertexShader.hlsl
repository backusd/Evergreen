
cbuffer MatrixBuffer : register(b0)
{
    matrix worldViewProjectionMatrix;
};

struct VertexInput
{
    float3 position : POSITION;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = mul(float4(input.position, 1.0f), worldViewProjectionMatrix);
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}