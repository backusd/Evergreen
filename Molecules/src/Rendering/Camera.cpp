#include "Camera.h"

using namespace Evergreen;
using namespace DirectX;

Camera::Camera(Viewport* viewport) :
    m_viewport(viewport),
	m_eye{ 0.0f, 0.0f, -5.0f },
	m_at{ 0.0f, 0.0f, 0.0f },
	m_up{ 0.0f, 1.0f, 0.0f },
    m_leftArrow(false),
    m_rightArrow(false),
    m_upArrow(false),
    m_downArrow(false),
    m_shift(false),
    m_ctrl(false),
    m_mouseLButtonDown(false),
    m_mousePositionX(0.0f),
    m_mousePositionY(0.0f),
    m_mousePositionXNew(0.0f),
    m_mousePositionYNew(0.0f),
    m_movingToNewLocation(false),
    m_eyeTarget{0.0f, 0.0f, 0.0f},
    m_eyeInitial{ 0.0f, 0.0f, 0.0f },
    m_upTarget{ 0.0f, 0.0f, 0.0f },
    m_upInitial{ 0.0f, 0.0f, 0.0f },
    m_moveStartTime(0.0),
    m_movementComplete(0.0),
    m_movementDuration(0.0),
    m_timeAtLastMoveUpdate(0.0),
    m_totalRotationAngle(0.0),
    m_rotatingLeftRight(false),
    m_rotatingUpDown(false)
{
    EG_ASSERT(viewport != nullptr, "Viewport cannot be nullptr");
	CreateProjectionMatrix(viewport->GetAspectRatio());
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
    if (m_mouseLButtonDown)
    {
        // Cancel out any existing automated movement
        m_movingToNewLocation = false;

        // If the pointer were to move from the middle of the screen to the far right,
        // that should produce one full rotation. Therefore, set a rotationFactor = 2
        const D3D11_VIEWPORT& vp = m_viewport->GetViewport();
        constexpr float rotationFactor = 2.0f;
        float radiansPerPixelX = (DirectX::XM_2PI / vp.Width) * rotationFactor;
        float radiansPerPixelY = (DirectX::XM_2PI / vp.Height) * rotationFactor;

        float thetaX = radiansPerPixelX * (m_mousePositionXNew - m_mousePositionX);
        float thetaY = radiansPerPixelY * (m_mousePositionYNew - m_mousePositionY);

        // Rotate
        RotateLeftRight(thetaX);
        RotateUpDown(thetaY);

        // reset the mouse position variables
        m_mousePositionX = m_mousePositionXNew;
        m_mousePositionY = m_mousePositionYNew;
    }
    else if (m_upArrow || m_downArrow || m_leftArrow || m_rightArrow)
    {
        // Cancel out any existing automated movement
        m_movingToNewLocation = false;

        // Compute the rotation
        constexpr float radiansPerSecond = 0.5f;
        float theta = static_cast<float>(timer.GetElapsedSeconds() * radiansPerSecond);

        // If rotating up or left, make the angle negative so the rest of the math is the same
        if (m_upArrow || m_leftArrow)
            theta *= -1;

        if (m_upArrow || m_downArrow)
            RotateUpDown(theta);

        if (m_leftArrow || m_rightArrow)
            RotateLeftRight(theta);
    }
    else if (m_movingToNewLocation)
    {
        if (m_movementComplete)
        {
            m_movingToNewLocation = false;
            m_rotatingLeftRight = false;
            m_rotatingUpDown = false;
            return;
        }

        // If the move start time is less than 0, it needs to be set
        if (m_moveStartTime < 0.0)
        {
            m_moveStartTime = timer.GetTotalSeconds();
            m_timeAtLastMoveUpdate = m_moveStartTime;
            return;
        }

        double currentTime = timer.GetTotalSeconds();
        double timeDelta = timer.GetElapsedSeconds();

        // If rotating left/right, just compute the necessary angle and call RotateLeftRight / RotateUpDown
        if (m_rotatingLeftRight)
        {
            // If we have surpassed the time for the movement, then use a truncated timeDelta & signal that we are done
            if (m_moveStartTime + m_movementDuration < currentTime)
            {
                m_movementComplete = true;
                timeDelta = m_moveStartTime + m_movementDuration - m_timeAtLastMoveUpdate;
            }

            float theta = m_totalRotationAngle * static_cast<float>(timeDelta / m_movementDuration); 

            RotateLeftRight(theta);

            m_timeAtLastMoveUpdate = currentTime;
        }
        else if (m_rotatingUpDown)
        {
            // If we have surpassed the time for the movement, then use a truncated timeDelta & signal that we are done
            if (m_moveStartTime + m_movementDuration < currentTime)
            {
                m_movementComplete = true;
                timeDelta = m_moveStartTime + m_movementDuration - m_timeAtLastMoveUpdate;
            }

            float theta = m_totalRotationAngle * static_cast<float>(timeDelta / m_movementDuration);

            RotateUpDown(theta);

            m_timeAtLastMoveUpdate = currentTime;
        }
        else
        {
            // Compute the ratio of elapsed time / allowed time to complete
            float timeRatio = static_cast<float>((currentTime - m_moveStartTime) / m_movementDuration);

            // if the current time is passed the movement duration, just assign final postion
            if (timeRatio >= 1.0f)
            {
                m_movementComplete = true;
                m_eye = DirectX::XMLoadFloat3(&m_eyeTarget);
                m_up = DirectX::XMLoadFloat3(&m_upTarget);
            }
            else
            {
                // Compute the intermediate position
                XMFLOAT3 eyeCurrent;
                eyeCurrent.x = m_eyeInitial.x + ((m_eyeTarget.x - m_eyeInitial.x) * timeRatio);
                eyeCurrent.y = m_eyeInitial.y + ((m_eyeTarget.y - m_eyeInitial.y) * timeRatio);
                eyeCurrent.z = m_eyeInitial.z + ((m_eyeTarget.z - m_eyeInitial.z) * timeRatio);

                m_eye = DirectX::XMLoadFloat3(&eyeCurrent);

                // Compute the intermediate position
                XMFLOAT3 upCurrent;
                upCurrent.x = m_upInitial.x + ((m_upTarget.x - m_upInitial.x) * timeRatio);
                upCurrent.y = m_upInitial.y + ((m_upTarget.y - m_upInitial.y) * timeRatio);
                upCurrent.z = m_upInitial.z + ((m_upTarget.z - m_upInitial.z) * timeRatio);

                m_up = DirectX::XMLoadFloat3(&upCurrent);
            }
        }
    }
}



void Camera::InitializeAutomatedMove(double maxMoveTime) noexcept
{
    // Set m_movingToNewLocation = true so the renderer knows to update the view matrix
    m_movingToNewLocation = true;

    // Set the move completed flag to false
    m_movementComplete = false;

    // Reset the start time to -1 to signal it needs to be set in the next Update
    m_moveStartTime = -1.0;

    // Set the movement max time to 0.1 seconds, so the zoom completes in that time
    m_movementDuration = maxMoveTime;

    DirectX::XMStoreFloat3(&m_eyeInitial, m_eye);
    DirectX::XMStoreFloat3(&m_upInitial, m_up);
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

void Camera::CenterOnFace() noexcept
{
    // Set automated move flags and initial data - 0.5 seconds for the move
    InitializeAutomatedMove(0.5);

    // Determine the coordinate with the max value and 0 out the other ones
    m_eyeTarget.x = m_eyeInitial.x;
    m_eyeTarget.y = m_eyeInitial.y;
    m_eyeTarget.z = m_eyeInitial.z;

    XMFLOAT3 length3;
    DirectX::XMStoreFloat3(&length3, DirectX::XMVector3Length(m_eye));
    float length = length3.x;

    m_eyeTarget.x = (std::abs(m_eyeInitial.x) < std::abs(m_eyeInitial.y) || std::abs(m_eyeInitial.x) < std::abs(m_eyeInitial.z)) ? 0.0f : length;
    m_eyeTarget.y = (std::abs(m_eyeInitial.y) < std::abs(m_eyeInitial.x) || std::abs(m_eyeInitial.y) < std::abs(m_eyeInitial.z)) ? 0.0f : length;
    m_eyeTarget.z = (std::abs(m_eyeInitial.z) < std::abs(m_eyeInitial.x) || std::abs(m_eyeInitial.z) < std::abs(m_eyeInitial.y)) ? 0.0f : length;

    m_eyeTarget.x *= (m_eyeInitial.x < 0.0f) ? -1.0f : 1.0f;
    m_eyeTarget.y *= (m_eyeInitial.y < 0.0f) ? -1.0f : 1.0f;
    m_eyeTarget.z *= (m_eyeInitial.z < 0.0f) ? -1.0f : 1.0f;

    // Determine the coordinate with the max value and 0 out the other ones
    // Whichever coordinate for the eye target is used must not be used for the up target, so zero it out
    float xInit = (m_eyeTarget.x == 0.0f) ? m_upInitial.x : 0.0f;
    float yInit = (m_eyeTarget.y == 0.0f) ? m_upInitial.y : 0.0f;
    float zInit = (m_eyeTarget.z == 0.0f) ? m_upInitial.z : 0.0f;

    DirectX::XMStoreFloat3(&length3, DirectX::XMVector3Length(m_up));
    length = length3.x;

    m_upTarget.x = (std::abs(xInit) < std::abs(yInit) || std::abs(xInit) < std::abs(zInit)) ? 0.0f : length;
    m_upTarget.y = (std::abs(yInit) < std::abs(xInit) || std::abs(yInit) < std::abs(zInit)) ? 0.0f : length;
    m_upTarget.z = (std::abs(zInit) < std::abs(xInit) || std::abs(zInit) < std::abs(yInit)) ? 0.0f : length;

    m_upTarget.x *= (xInit < 0.0f) ? -1.0f : 1.0f;
    m_upTarget.y *= (yInit < 0.0f) ? -1.0f : 1.0f;
    m_upTarget.z *= (zInit < 0.0f) ? -1.0f : 1.0f;
}
void Camera::RotateLeft90() noexcept
{
    // Only allow a single left/right movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingLeftRight = true;
        m_totalRotationAngle = -1.0f * DirectX::XM_PIDIV2;
    }
}
void Camera::RotateRight90() noexcept
{
    // Only allow a single left/right movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingLeftRight = true;
        m_totalRotationAngle = DirectX::XM_PIDIV2;
    }
}
void Camera::RotateUp90() noexcept
{
    // Only allow a single up/down movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingUpDown = true;
        m_totalRotationAngle = DirectX::XM_PIDIV2;
    }
}
void Camera::RotateDown90() noexcept
{
    // Only allow a single up/down movement at a time
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.5 seconds for the move
        InitializeAutomatedMove(0.5);
        m_rotatingUpDown = true;
        m_totalRotationAngle = -1.0f * DirectX::XM_PIDIV2;
    }
}


void Camera::OnLButtonPressed(float x, float y) noexcept
{
    // When the pointer is pressed begin tracking the pointer movement.
    m_mouseLButtonDown = true;
    m_mousePositionX = m_mousePositionXNew = x;
    m_mousePositionY = m_mousePositionYNew = y;
}
void Camera::OnRButtonPressed(float x, float y) noexcept
{

}
void Camera::OnLButtonReleased(float x, float y) noexcept
{
    // When the pointer is pressed begin tracking the pointer movement.
    m_mouseLButtonDown = false;
    m_mousePositionX = m_mousePositionXNew = x;
    m_mousePositionY = m_mousePositionYNew = y;
}
void Camera::OnRButtonReleased(float x, float y) noexcept
{

}
void Camera::OnLButtonDoubleClick(float x, float y) noexcept
{
    // Set automated move flags and initial data - 0.5 seconds for the move
    InitializeAutomatedMove(0.5);

    // Set eye target location to half the distance to the center
    m_eyeTarget.x = m_eyeInitial.x / 2.0f;
    m_eyeTarget.y = m_eyeInitial.y / 2.0f;
    m_eyeTarget.z = m_eyeInitial.z / 2.0f;

    m_upTarget = m_upInitial;
}
void Camera::OnRButtonDoubleClick(float x, float y) noexcept
{

}
void Camera::OnMouseMove(float x, float y) noexcept
{
    m_mousePositionXNew = x;
    m_mousePositionYNew = y;
}
void Camera::OnMouseScrolledVertical(float x, float y, int scrollDelta) noexcept
{
    // Only update if not already moving (this avoids a flood of WM_MOUSEWHEEL messages)
    if (!m_movingToNewLocation)
    {
        // Set automated move flags and initial data - 0.1 seconds for the move
        InitializeAutomatedMove(0.1);

        m_upTarget = m_upInitial;

        // Set eye target location to be 10% closer/further than the current location
        float factor = (scrollDelta > 0) ? 1.1f : 0.9f;

        m_eyeTarget.x = m_eyeInitial.x * factor;
        m_eyeTarget.y = m_eyeInitial.y * factor;
        m_eyeTarget.z = m_eyeInitial.z * factor;
    }
}
void Camera::OnMouseScrolledHorizontal(float x, float y, int scrollDelta) noexcept
{

}

void Camera::OnChar(char c) noexcept
{
    switch (c)
    {
    case 'c': CenterOnFace(); break;
    case 'w': RotateUp90(); break;
    case 'a': RotateRight90(); break;
    case 's': RotateDown90(); break;
    case 'd': RotateLeft90(); break;
    }
}
void Camera::OnKeyPressed(Evergreen::KEY_CODE key) noexcept
{
    switch (key)
    {
    case KEY_CODE::EG_LEFT_ARROW: m_leftArrow = true; break;
    case KEY_CODE::EG_RIGHT_ARROW: m_rightArrow = true; break;
    case KEY_CODE::EG_UP_ARROW: m_upArrow = true; break;
    case KEY_CODE::EG_DOWN_ARROW: m_downArrow = true; break;
    case KEY_CODE::EG_SHIFT: m_shift = true; break;
    case KEY_CODE::EG_CTRL: m_ctrl = true; break;
    default:
        break;
    }
}
void Camera::OnKeyReleased(Evergreen::KEY_CODE key) noexcept
{
    switch (key)
    {
    case KEY_CODE::EG_LEFT_ARROW: m_leftArrow = false; break;
    case KEY_CODE::EG_RIGHT_ARROW: m_rightArrow = false; break;
    case KEY_CODE::EG_UP_ARROW: m_upArrow = false; break;
    case KEY_CODE::EG_DOWN_ARROW: m_downArrow = false; break;
    case KEY_CODE::EG_SHIFT: m_shift = false; break;
    case KEY_CODE::EG_CTRL: m_ctrl = false; break;
    default:
        break;
    }
}