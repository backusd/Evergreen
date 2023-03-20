#pragma once
#include "pch.h"



class Camera
{
public:

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMFLOAT3 GetPosition() const;

private:
	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV4;
	float m_radius = 5.0f;
};