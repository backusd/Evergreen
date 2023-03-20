//cbuffer cbPerObject : register(b0)
//{
//    float4x4 gWorldViewProj;
//};
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
};

struct VSIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
};

struct VSOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VSOut main(VSIn vin)
{
    VSOut vout;
	
	// Transform to homogeneous clip space.
    //vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}