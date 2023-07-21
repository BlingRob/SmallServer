#include "utils.h"

#include "fstream"

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
            throw(std::string(exc.what()) + "\n File:" + std::string(path) +
                  " doesn't exist!");
        }

    if (!file.is_open())
        throw("File isn't opened!");

    std::streamsize fileSize = file.tellg();
    file.seekg(0);
    std::string text(fileSize + 1, '\0');
    file.read(&text[0], fileSize);

    return text;
}