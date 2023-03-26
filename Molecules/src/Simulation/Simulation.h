#pragma once
#include "pch.h"
#include <Evergreen.h>

enum class Element
{
	Null = 0,
	Hydrogen = 1,
	Helium = 2,
	Lithium = 3,
	Beryllium = 4,
	Boron = 5,
	Carbon = 6,
	Nitrogen = 7,
	Oxygen = 8,
	Flourine = 9,
	Neon = 10
};

constexpr std::array<float, 11> AtomicRadii{
	{
		0.0f,	// Invalid value to take up the 0 index spot
		0.025f,	// Hydrogen
		0.120f,	// Helium
		0.145f,	// Lithium
		0.105f, // Beryllium
		0.085f, // Boron
		0.070f, // Carbon
		0.065f, // Nitrogen
		0.060f, // Oxygen
		0.050f, // Flourine
		0.160f  // Neon
	}
};



class Simulation
{
public:
	Simulation() noexcept;

	void Play() noexcept { m_isPaused = false; }
	void Pause() noexcept { m_isPaused = true; }

	void Add(Element element, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity) noexcept;

	void Update(const Evergreen::Timer& timer);

	std::vector<DirectX::XMFLOAT3>& Positions() noexcept { return m_positions; }
	std::vector<DirectX::XMFLOAT3>& Velocities() noexcept { return m_velocities; }
	std::vector<Element>& ElementTypes() noexcept { return m_elementTypes; }

private:
	std::vector<DirectX::XMFLOAT3> m_positions;
	std::vector<DirectX::XMFLOAT3> m_velocities;
	std::vector<Element> m_elementTypes;

	bool m_isPaused;
	double m_elapsedTime;

	float m_boxMax;
};