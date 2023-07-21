#pragma once

#include <string>

namespace HTTP_ANSWERS
{
	const std::string notFound("HTTP/1.0 404 Not Found\r\nContent-length: 0\r\nContent-Type: text/html\r\n\r\n");
	const std::string ok("HTTP/1.0 200 OK\r\n");
};