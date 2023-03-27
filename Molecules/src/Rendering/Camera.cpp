#include "Camera.h"

using namespace Evergreen;
using namespace DirectX;

Camera::Camera(float aspectRatio) :
	m_eye{ 0.0f, 0.0f, -5.0f },
	m_at{ 0.0f, 0.0f, 0.0f },
	m_up{ 0.0f, 1.0f, 0.0f },
    m_leftArrowDown(false),
    m_rightArrowDown(false)
{
	CreateProjectionMatrix(aspectRatio);
}

XMMATRIX Camera::ViewMatrix() const noexcept
{
	return DirectX::XMMatrixLookAtLH(m_eye, m_at, m_up);
}
XMMATRIX Camera::ProjectionMatrix() const noexcept
{
	return m_projectionMatrix;
}
void Camera::CreateProjectionMatrix(float aspectRatio) noexcept
{
    // Perspective Matrix
    float fovAngleY = DirectX::XM_PI / 4;

    // This is a simple example of a change that can be made when the app is in portrait or snapped view
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    //XMFLOAT4X4 orientation = m_deviceResources->OrientationTransform3D();
    //XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);
    XMMATRIX orientationMatrix = DirectX::XMMatrixIdentity();

    // Projection Matrix
    m_projectionMatrix = perspectiveMatrix * orientationMatrix;
}

XMFLOAT3 Camera::Position() const noexcept
{
	XMFLOAT3 position;
	DirectX::XMStoreFloat3(&position, m_eye);
	return position;
}

void Camera::Update(const Timer& timer)
{
    double timeDelta = timer.GetElapsedSeconds();

    float radiansPerSecond = 0.5;
    float theta = static_cast<float>(timeDelta * radiansPerSecond);

    if (m_leftArrowDown && !m_rightArrowDown)
    {
        // Rotate Left
        RotateLeftRight(theta);
    }
    else if (m_rightArrowDown && !m_leftArrowDown)
    {
        // Rotate Right
        RotateLeftRight(-theta);
    }
}




void Camera::RotateLeftRight(float theta) noexcept
{
    // Use Rodrigue's Rotation Formula
        //     See here: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
        //     v_rot : vector after rotation
        //     v     : vector before rotation
        //     theta : angle of rotation
        //     k     : unit vector representing axis of rotation
        //     v_rot = v*cos(theta) + (k x v)*sin(theta) + k*(k dot v)*(1-cos(theta))

    XMVECTOR v = m_eye;
    XMVECTOR k = m_up;
    m_eye = v * cos(theta) + DirectX::XMVector3Cross(k, v) * sin(theta) + k * DirectX::XMVector3Dot(k, v) * (1 - cos(theta));

    // Do NOT change the up-vector
}

void Camera::RotateUpDown(float theta) noexcept
{
    // Use Rodrigue's Rotation Formula
    //     See here: https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    //     v_rot : vector after rotation
    //     v     : vector before rotation
    //     theta : angle of rotation
    //     k     : unit vector representing axis of rotation
    //     v_rot = v*cos(theta) + (k x v)*sin(theta) + k*(k dot v)*(1-cos(theta))


    // The axis of rotation vector for up/down rotation will be the cross product 
    // between the eye-vector and the up-vector (must make it a unit vector)
    XMVECTOR v = m_eye;
    XMVECTOR k = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_eye, m_up));
    m_eye = v * cos(theta) + DirectX::XMVector3Cross(k, v) * sin(theta) + k * DirectX::XMVector3Dot(k, v) * (1 - cos(theta));

    // Now update the new up-vector should be the cross product between the k-vector and the new eye-vector
    m_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(k, m_eye));
}