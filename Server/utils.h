/// \file utils.h
/// \brief Useful function's interfaces

#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>

namespace utils
{

constexpr uint64_t kByte = 1024;
constexpr uint64_t mByte = 1024 * 1024;
constexpr uint64_t gByte = 1024 * 1024 * 1024;


/// \brief Read text file
/// \param[in] path path to file
std::string LoadTextFile(std::string_view path);

/// @brief Get current date/time
/// @return string with format YYYY-MM-DD-HH:mm:ss
const std::string CurrentDateTime();

} // namespace utils

#endif // UTILS_H