#pragma once
#include "pch.h"
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace Evergreen
{
// Drop this warning because the private static shared_ptr's are private, but the compiler will complain
// that they don't have a DLL interface
// See: https://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Log
{
public:
	static void Init() noexcept;

	ND inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() noexcept { return m_coreLogger; }
	ND inline static std::shared_ptr<spdlog::logger>& GetClientLogger() noexcept { return m_clientLogger; }

private:
	static std::shared_ptr<spdlog::logger> m_coreLogger;
	static std::shared_ptr<spdlog::logger> m_clientLogger;
};
#pragma warning( pop )

}

// Core Log Macros
#define EG_CORE_ERROR(fmt, ...) ::Evergreen::Log::GetCoreLogger()->error(std::format(fmt, __VA_ARGS__))
#define EG_CORE_WARN(fmt, ...)  ::Evergreen::Log::GetCoreLogger()->warn(std::format(fmt, __VA_ARGS__))
#define EG_CORE_INFO(fmt, ...)  ::Evergreen::Log::GetCoreLogger()->info(std::format(fmt, __VA_ARGS__))
#define EG_CORE_TRACE(fmt, ...) ::Evergreen::Log::GetCoreLogger()->trace(std::format(fmt, __VA_ARGS__))

// Client Log Macros
#define EG_ERROR(fmt, ...) ::Evergreen::Log::GetClientLogger()->error(std::format(fmt, __VA_ARGS__))
#define EG_WARN(fmt, ...)  ::Evergreen::Log::GetClientLogger()->warn(std::format(fmt, __VA_ARGS__))
#define EG_INFO(fmt, ...)  ::Evergreen::Log::GetClientLogger()->info(std::format(fmt, __VA_ARGS__))
#define EG_TRACE(fmt, ...) ::Evergreen::Log::GetClientLogger()->trace(std::format(fmt, __VA_ARGS__))
