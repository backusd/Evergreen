#pragma once
#include "pch.h"
#include <Evergreen.h>

class Camera
{
public:
	Camera(Evergreen::Viewport* viewport);

	void Update(const Evergreen::Timer& timer);

	ND DirectX::XMMATRIX ViewMatrix() const noexcept;
	ND DirectX::XMMATRIX ProjectionMatrix() const noexcept;
	ND DirectX::XMFLOAT3 Position() const noexcept;

	inline void SetAspectRatio(float aspectRatio) noexcept { CreateProjectionMatrix(aspectRatio); }

	void OnLButtonPressed(float x, float y) noexcept;
	void OnRButtonPressed(float x, float y) noexcept;
	void OnLButtonDoubleClick(float x, float y) noexcept;
	void OnRButtonDoubleClick(float x, float y) noexcept;
	void OnLButtonReleased(float x, float y) noexcept;
	void OnRButtonReleased(float x, float y) noexcept;
	void OnMouseMove(float x, float y) noexcept;
	void OnMouseScrolledVertical(float x, float y, int scrollDelta) noexcept;
	void OnMouseScrolledHorizontal(float x, float y, int scrollDelta) noexcept;

	void OnChar(char c) noexcept;
	void OnKeyPressed(Evergreen::KEY_CODE key) noexcept;
	void OnKeyReleased(Evergreen::KEY_CODE key) noexcept;

private:
	void CreateProjectionMatrix(float aspectRatio) noexcept;

	void InitializeAutomatedMove(double maxMoveTime) noexcept;
	void RotateLeftRight(float theta) noexcept;
	void RotateUpDown(float theta) noexcept;

	void CenterOnFace() noexcept;
	void RotateLeft90() noexcept;
	void RotateRight90() noexcept;
	void RotateUp90() noexcept;
	void RotateDown90() noexcept;

	// Viewport
	Evergreen::Viewport* m_viewport;

	// Eye/at/up vectors
	DirectX::XMVECTOR m_eye;
	DirectX::XMVECTOR m_at;
	DirectX::XMVECTOR m_up;

	DirectX::XMMATRIX m_projectionMatrix;

	// Input states for keyboard
	bool  m_leftArrow;
	bool  m_rightArrow;
	bool  m_upArrow;
	bool  m_downArrow;
	bool  m_shift;
	bool  m_ctrl;

	// Mouse Variables
	bool  m_mouseLButtonDown;
	float m_mousePositionX;
	float m_mousePositionY;
	float m_mousePositionXNew;
	float m_mousePositionYNew;

	// Automated move variables
	//		When zooming in/out, or rotating 90, etc., we simply need to set the target location and
	//		Update() will move the camera for us
	bool	m_movingToNewLocation;
	DirectX::XMFLOAT3 m_eyeTarget;
	DirectX::XMFLOAT3 m_eyeInitial;
	DirectX::XMFLOAT3 m_upTarget;
	DirectX::XMFLOAT3 m_upInitial;
	double m_moveStartTime;
	double m_movementComplete;
	double m_movementDuration;
	double m_timeAtLastMoveUpdate;
	float  m_totalRotationAngle;
	bool m_rotatingLeftRight;
	bool m_rotatingUpDown;
};