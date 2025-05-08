/// \file utils.cpp
/// \brief Useful function's implementation

#include "utils.h"

#include <fstream>
#include <format>
#include <time.h>

namespace utils
{

std::string LoadTextFile(std::string_view path)
{
    std::ifstream file;

    file.exceptions(std::ifstream::failbit);

    try
        {
            file.open(path.data(), std::ios::binary | std::ios::ate);
        }
    catch (const std::exception& exc)
        {
            throw(std::format("{}\nFile: {} doesn't exist!", exc.what(), path));
        }

    if (!file.is_open())
        throw("File isn't opened!");

    std::streamsize fileSize = file.tellg();
    file.seekg(0);
    std::string text(fileSize + 1, '\0');
    file.read(&text[0], fileSize);

    return text;
}

const std::string CurrentDateTime() 
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y.%m.%d-%X", &tstruct);

    return buf;
}

// template<typename... Args>
// std::string format(std::string_view rt_fmt_str, Args&&... args)
// {
//     return std::vformat(rt_fmt_str, std::make_format_args(args...));
// }

} // namespace utils