#include "Simulation.h"



Simulation::Simulation() noexcept :
	m_isPaused(true),
	m_boxMax(1.0f)
{}

void Simulation::Add(Element element, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity) noexcept
{
	m_elementTypes.push_back(element);
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}

void Simulation::Update(const Evergreen::Timer& timer)
{
	EG_ASSERT(m_positions.size() == m_velocities.size(), "Invalid");
	EG_ASSERT(m_positions.size() == m_elementTypes.size(), "Invalid");

	double timeDelta = timer.GetElapsedSeconds();

	if (m_isPaused)
		return;

	if (timeDelta > 0.1)
		return;

	for (unsigned int iii = 0; iii < m_positions.size(); ++iii)
	{
		m_positions[iii].x += m_velocities[iii].x * timeDelta;
		if (m_positions[iii].x + AtomicRadii[iii] > m_boxMax || m_positions[iii].x - AtomicRadii[iii] < -m_boxMax)
			m_velocities[iii].x *= -1;

		m_positions[iii].y += m_velocities[iii].y * timeDelta;
		if (m_positions[iii].y + AtomicRadii[iii] > m_boxMax || m_positions[iii].y - AtomicRadii[iii] < -m_boxMax)
			m_velocities[iii].y *= -1;

		m_positions[iii].z += m_velocities[iii].z * timeDelta;
		if (m_positions[iii].z + AtomicRadii[iii] > m_boxMax || m_positions[iii].z - AtomicRadii[iii] < -m_boxMax)
			m_velocities[iii].z *= -1;
	}
}