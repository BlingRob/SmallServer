#pragma once

#include <string_view>

static constexpr size_t kNSeverities{5};

enum class Severities
{
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};

struct ILogger
{
    /// @brief Set minimum severity for ouput
    /// @param sev Severity's value
    virtual void SetSeverities(Severities sev) = 0;

    /// @brief Logging message with marking severity
    /// @param msg logging message
    /// @param sev severity
    virtual void Log(std::string_view msg, Severities sev) = 0;
};
