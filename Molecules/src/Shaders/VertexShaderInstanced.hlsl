// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#define MAX_INSTANCES 100
#define NUM_MATERIALS 2

// Include structures and functions for lighting.
#include "Lighting.hlsl"

cbuffer cbPass : register(b0)
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
    float4 gAmbientLight;
    
    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld[MAX_INSTANCES];
};

//struct VSIn
//{
//    float3 PosL : POSITION;
//    uint MaterialIndex : MATERIAL_INDEX;
//    float3 NormalL : NORMAL;
//};
struct VSIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
};

struct VSOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    uint MaterialIndex : MATERIAL_INDEX;
    float3 NormalW : NORMAL;
    uint Instance_ID : INSTANCE_ID;
};

VSOut main(VSIn vin, uint materialIndex : MATERIAL_INDEX, uint instanceID : SV_InstanceID)
{
    VSOut vout;
    
    // Look up the world matrix in the list of all world matrices
    float4x4 world = gWorld[instanceID];
    
    // Just forward the material index and instance ID
    vout.MaterialIndex = materialIndex;
    vout.Instance_ID = instanceID;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)world);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);

    return vout;
}