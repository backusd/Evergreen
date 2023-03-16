cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
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
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}