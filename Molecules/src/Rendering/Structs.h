#pragma once
#include "pch.h"
#include "../Utils/MathHelper.h"

// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
struct Material
{
    // Material constant buffer data used for shading.
    DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
    float Shininess = .75f;

    //DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct Light
{
    DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
    float FalloffStart = 1.0f;                          // point/spot light only
    DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
    float FalloffEnd = 10.0f;                           // point/spot light only
    DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
    float SpotPower = 64.0f;                            // spot light only
};

#define MaxLights 16

//struct Vertex
//{
//	DirectX::XMFLOAT3 Pos;
//    DirectX::XMFLOAT3 Normal;
//};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};

struct PassConstants
{
    DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
    DirectX::XMFLOAT3   EyePosW = { 0.0f, 0.0f, 0.0f };
    float               cbPerObjectPad1 = 0.0f;
    DirectX::XMFLOAT2   RenderTargetSize = { 0.0f, 0.0f };
    DirectX::XMFLOAT2   InvRenderTargetSize = { 0.0f, 0.0f };
    float               NearZ = 0.0f;
    float               FarZ = 0.0f;
    float               TotalTime = 0.0f;
    float               DeltaTime = 0.0f;

    DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light Lights[MaxLights];
};

// ------------------------------------------------------------------------

#define MAX_INSTANCES 100
#define NUM_MATERIALS 2

struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    //UINT MaterialIndex;
    DirectX::XMFLOAT3 Normal;
};

struct WorldMatrixInstances
{
    DirectX::XMFLOAT4X4 worlds[MAX_INSTANCES];
};

struct MaterialsArray
{
    Material materials[NUM_MATERIALS];
};


// For BOX rendering
struct WorldViewProjectionMatrix
{
    DirectX::XMFLOAT4X4 worldViewProjection;
};