#pragma once

#include "logger_interface.h"

#include <string>
#include <string_view>

/// @brief Logging outputs 
/*
    BOOST_LOG_TRIVIAL(trace) << "This is a trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "This is a debug severity message";
    BOOST_LOG_TRIVIAL(info) << "This is an informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "This is a warning severity message";
    BOOST_LOG_TRIVIAL(error) << "This is an error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "and this is a fatal severity message";
*/

class BoostLogger : public ILogger
{
    public:

    /// @param logFilePath Log's file name
    BoostLogger(const std::string& logFilePath = "");

    void SetSeverities(Severities sev) override;
    
    void Log(std::string_view msg, Severities sev) override;

    private:

    /// @brief Min output severity
    Severities sev_;
};