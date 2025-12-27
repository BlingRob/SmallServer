#pragma once

#include <cinttypes>
#include <string>

namespace default_configures
{

const std::string kHost{"127.0.0.1"};

const std::uint16_t kHttpPort{15000};

const std::uint16_t kFtpPort{15001};

const std::string kHttpWorkDir = "./";

const std::string kFtpWorkDir = "./";

const bool kAsConsole{false};

}  // namespace default_configures