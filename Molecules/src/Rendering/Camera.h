#pragma once
#include "pch.h"
#include <Evergreen.h>

class Camera
{
public:
	Camera(float aspectRatio);

	void Update(const Evergreen::Timer& timer);

	ND DirectX::XMMATRIX ViewMatrix() const noexcept;
	ND DirectX::XMMATRIX ProjectionMatrix() const noexcept;
	ND DirectX::XMFLOAT3 Position() const noexcept;

	inline void SetAspectRatio(float aspectRatio) noexcept { CreateProjectionMatrix(aspectRatio); }

	inline void LeftArrowDown(bool isDown) noexcept { m_leftArrowDown = isDown; }
	inline void RightArrowDown(bool isDown) noexcept { m_rightArrowDown = isDown; }

private:
	void CreateProjectionMatrix(float aspectRatio) noexcept;

	void RotateLeftRight(float theta) noexcept;
	void RotateUpDown(float theta) noexcept;

	// Eye/at/up vectors
	DirectX::XMVECTOR m_eye;
	DirectX::XMVECTOR m_at;
	DirectX::XMVECTOR m_up;

	DirectX::XMMATRIX m_projectionMatrix;


	// Mouse State variables
	bool m_leftArrowDown;
	bool m_rightArrowDown;
};