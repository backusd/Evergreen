#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <memory>

namespace Evergreen
{
// Drop this warning because the private static shared_ptr's are private, but the compiler will complain
// that they don't have a DLL interface
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class EVERGREEN_API Log
{
public:
	static void Init() noexcept;

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() noexcept { return m_coreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() noexcept { return m_clientLogger; }

private:
	static std::shared_ptr<spdlog::logger> m_coreLogger;
	static std::shared_ptr<spdlog::logger> m_clientLogger;
};
#pragma warning( pop )

}

// Core Log Macros
#define EG_CORE_ERROR(...) ::Evergreen::Log::GetCoreLogger()->error(__VA_ARGS__)
#define EG_CORE_WARN(...)  ::Evergreen::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define EG_CORE_INFO(...)  ::Evergreen::Log::GetCoreLogger()->info(__VA_ARGS__)
#define EG_CORE_TRACE(...) ::Evergreen::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define EG_CORE_FATAL(...) ::Evergreen::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client Log Macros
#define EG_ERROR(...) ::Evergreen::Log::GetClientLogger()->error(__VA_ARGS__)
#define EG_WARN(...)  ::Evergreen::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EG_INFO(...)  ::Evergreen::Log::GetClientLogger()->info(__VA_ARGS__)
#define EG_TRACE(...) ::Evergreen::Log::GetClientLogger()->trace(__VA_ARGS__)
#define EG_FATAL(...) ::Evergreen::Log::GetClientLogger()->fatal(__VA_ARGS__)