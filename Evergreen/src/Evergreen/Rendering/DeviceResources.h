#pragma once


#ifdef EG_DX11
#include "Evergreen/Rendering/DX11/DeviceResourcesDX11.h"
#include "Evergreen/Rendering/DX11/DeviceResourcesExceptionDX11.h"
#define DeviceResources DeviceResourcesDX11
#elif EG_DX12
#include "Evergreen/Rendering/DX12/DeviceResourcesDX12.h"
#include "Evergreen/Rendering/DX12/DeviceResourcesExceptionDX12.h"
#define DeviceResources DeviceResourcesDX12
#elif EG_OPENGL
#include "Evergreen/Rendering/OpenGL/DeviceResourcesOpenGL.h"
#define DeviceResources DeviceResourcesOpenGL
#elif EG_VULKAN
#include "Evergreen/Rendering/Vulkan/DeviceResourcesVulkan.h"
#define DeviceResources DeviceResourcesVulkan
#endif