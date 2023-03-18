#pragma once
#include "pch.h"
#include "../Utils/MathHelper.h"
#include <Evergreen.h>




class PipelineConfig
{
public:
	PipelineConfig(std::shared_ptr<Evergreen::DeviceResources> deviceResources);
	void ApplyConfig();

private:
	std::shared_ptr<Evergreen::DeviceResources> m_deviceResources;

};