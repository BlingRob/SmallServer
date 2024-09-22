/// \file utils.h
/// \brief Useful function's interfaces

#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <string_view>

/// \brief Read text file
/// \param[in] path path to file
std::string LoadTextFile(std::string_view path);

#endif // UTILS_H