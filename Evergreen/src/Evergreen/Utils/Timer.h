#pragma once
#include "pch.h"
#include "Evergreen/Core.h"
#include "Evergreen/Log.h"

namespace Evergreen
{
// Drop this warning because the private members are not accessible by the client application, but 
// the compiler will complain that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Timer
{
public:
	Timer() noexcept :
		m_elapsedTicks(0),
		m_totalTicks(0),
		m_leftOverTicks(0),
		m_frameCount(0),
		m_framesPerSecond(0),
		m_framesThisSecond(0),
		m_qpcSecondCounter(0),
		m_isFixedTimeStep(false),
		m_targetElapsedTicks(TicksPerSecond / 60)
	{
		if (!QueryPerformanceFrequency(&m_qpcFrequency))
		{
			EG_CORE_ERROR("{}:{} - Call to QueryPerformanceFrequency failed", __FILE__, __LINE__);
		}

		if (!QueryPerformanceCounter(&m_qpcLastTime))
		{
			EG_CORE_ERROR("{}:{} - Call to QueryPerformanceCounter failed", __FILE__, __LINE__);
		}

		// Initialize max delta to 1/10 of a second.
		m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
	}

	// Get elapsed time since the previous Update call.
	ND inline uint64_t GetElapsedTicks() const noexcept { return m_elapsedTicks; }
	ND inline double GetElapsedSeconds() const noexcept { return TicksToSeconds(m_elapsedTicks); }

	// Get total time since the start of the program.
	ND inline uint64_t GetTotalTicks() const noexcept { return m_totalTicks; }
	ND inline double GetTotalSeconds() const noexcept { return TicksToSeconds(m_totalTicks); }

	// Get total number of updates since start of the program.
	ND inline uint32_t GetFrameCount() const noexcept { return m_frameCount; }

	// Get the current framerate.
	ND inline uint32_t GetFramesPerSecond() const noexcept { return m_framesPerSecond; }

	// Set whether to use fixed or variable timestep mode.
	inline void SetFixedTimeStep(bool isFixedTimestep) noexcept { m_isFixedTimeStep = isFixedTimestep; }

	// Set how often to call Update when in fixed timestep mode.
	inline void SetTargetElapsedTicks(uint64_t targetElapsed) noexcept { m_targetElapsedTicks = targetElapsed; }
	inline void SetTargetElapsedSeconds(double targetElapsed) noexcept { m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

	// Integer format represents time using 10,000,000 ticks per second.
	static const uint64_t TicksPerSecond = 10000000;

	ND inline static double TicksToSeconds(uint64_t ticks) noexcept { return static_cast<double>(ticks) / TicksPerSecond; }
	ND inline static uint64_t SecondsToTicks(double seconds) noexcept { return static_cast<uint64_t>(seconds * TicksPerSecond); }

	// After an intentional timing discontinuity (for instance a blocking IO operation)
	// call this to avoid having the fixed timestep logic attempt a set of catch-up 
	// Update calls.

	void ResetElapsedTime()
	{
		if (!QueryPerformanceCounter(&m_qpcLastTime))
		{
			EG_CORE_ERROR("{}:{} - Call to QueryPerformanceCounter failed", __FILE__, __LINE__);
		}

		m_leftOverTicks = 0;
		m_framesPerSecond = 0;
		m_framesThisSecond = 0;
		m_qpcSecondCounter = 0;
	}

	// Update timer state, calling the specified Update function the appropriate number of times.
	template<typename TUpdate>
	void Tick(const TUpdate& update)
	{
		// Query the current time.
		LARGE_INTEGER currentTime;

		if (!QueryPerformanceCounter(&currentTime))
		{
			EG_CORE_ERROR("{}:{} - Call to QueryPerformanceCounter failed", __FILE__, __LINE__);
		}


		uint64_t timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

		m_qpcLastTime = currentTime;
		m_qpcSecondCounter += timeDelta;

		// Clamp excessively large time deltas (e.g. after paused in the debugger).
		if (timeDelta > m_qpcMaxDelta)
		{
			timeDelta = m_qpcMaxDelta;
		}

		// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
		timeDelta *= TicksPerSecond;
		timeDelta /= m_qpcFrequency.QuadPart;

		uint32_t lastFrameCount = m_frameCount;

		if (m_isFixedTimeStep)
		{
			// Fixed timestep update logic

			// If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
			// the clock to exactly match the target value. This prevents tiny and irrelevant errors
			// from accumulating over time. Without this clamping, a game that requested a 60 fps
			// fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
			// accumulate enough tiny errors that it would drop a frame. It is better to just round 
			// small deviations down to zero to leave things running smoothly.

			if (abs(static_cast<int64_t>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
			{
				timeDelta = m_targetElapsedTicks;
			}

			m_leftOverTicks += timeDelta;

			while (m_leftOverTicks >= m_targetElapsedTicks)
			{
				m_elapsedTicks = m_targetElapsedTicks;
				m_totalTicks += m_targetElapsedTicks;
				m_leftOverTicks -= m_targetElapsedTicks;
				m_frameCount++;

				{
					update();
				}
			}
		}
		else
		{
			// Variable timestep update logic.
			m_elapsedTicks = timeDelta;
			m_totalTicks += timeDelta;
			m_leftOverTicks = 0;
			m_frameCount++;

			{
				update();
			}
		}

		// Track the current framerate.
		if (m_frameCount != lastFrameCount)
		{
			m_framesThisSecond++;
		}

		if (m_qpcSecondCounter >= static_cast<uint64_t>(m_qpcFrequency.QuadPart))
		{
			m_framesPerSecond = m_framesThisSecond;
			m_framesThisSecond = 0;
			m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
		}
	}

private:
	// Source timing data uses QPC units.
	LARGE_INTEGER m_qpcFrequency;
	LARGE_INTEGER m_qpcLastTime;
	uint64_t m_qpcMaxDelta;

	// Derived timing data uses a canonical tick format.
	uint64_t m_elapsedTicks;
	uint64_t m_totalTicks;
	uint64_t m_leftOverTicks;

	// Members for tracking the framerate.
	uint32_t m_frameCount;
	uint32_t m_framesPerSecond;
	uint32_t m_framesThisSecond;
	uint64_t m_qpcSecondCounter;

	// Members for configuring fixed timestep mode.
	bool m_isFixedTimeStep;
	uint64_t m_targetElapsedTicks;
};
#pragma warning( pop )
}