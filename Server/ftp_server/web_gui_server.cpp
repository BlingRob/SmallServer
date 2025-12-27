#include "web_gui_server.h"

#include <boost/beast/version.hpp>
#include <filesystem>
#include <fstream>

#include "default_configure.h"
#include "utils.h"

namespace fs = std::filesystem;
namespace beast = boost::beast;

WebGuiFtpServer::WebGuiFtpServer(boost::asio::io_context& ioc, const toml::table& cfg, ILogger& logger)
    : BaseHttpServer(ioc,
                     {cfg["connections"]["host"].value_or(default_configures::kHost),
                      cfg["connections"]["http_port"].value_or(default_configures::kHttpPort)},
                     logger),
      ftp_root_(cfg["work_dirs"]["ftp"].value_or(default_configures::kFtpWorkDir))
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    auto host{cfg["connections"]["host"].value_or(default_configures::kHost)};
    auto port{cfg["connections"]["http_port"].value_or(default_configures::kHttpPort)};

    std::filesystem::path ftp_index_path(cfg["work_dirs"]["http"].value_or(default_configures::kHttpWorkDir));
    ftp_index_path /= "ftp_index.html";

    if (!std::filesystem::exists(ftp_index_path))
    {
        std::string error_msg{std::format("File {} not found", ftp_index_path.string())};
        logger_.Log(error_msg, Severities::Error);
        throw std::runtime_error{error_msg};
    }

    index_html_file_ = utils::LoadTextFile(ftp_index_path.string());

    logger_.Log(std::format("Http Server {}:{} for ftp server interface started", host, port), Severities::Info);
}

response_t WebGuiFtpServer::http_request_handler(http::request<http::string_body> req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    // Handle CORS preflight
    if (req.method() == http::verb::options)
    {
        return cors_response();
    }

    std::string target{req.target()};

    // API endpoints
    if (target == "/api/files")
    {
        return handle_get_files(req);
    }
    else if (target == "/api/upload" && req.method() == http::verb::post)
    {
        return handle_upload(req);
    }
    else if (target.find("/api/download/") == 0)
    {
        // return handle_download(req);
    }
    else if (target.find("/api/delete/") == 0)
    {
        return handle_delete(req);
    }
    else if (target.find("/api/mkdir") == 0)
    {
        return handle_mkdir(req);
    }
    else if (target.find("/api/rmdir/") == 0)
    {
        return handle_rmdir(req);
    }
    else if (target.find("/api/rename") == 0)
    {
        return handle_rename(req);
    }
    else
    {
        return serve_static_file(req);
    }
}

response_t WebGuiFtpServer::handle_get_files(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        std::string path{ftp_root_};

        // Check for path parameter
        auto params{parse_query(req.target())};
        if (params.find("path") != params.end())
        {
            std::string subpath{params["path"]};
            if (!subpath.empty() && subpath[0] == '/')
            {
                subpath = subpath.substr(1);
            }
            path = (fs::path(ftp_root_) / subpath).string();
        }

        if (!fs::exists(path) || !fs::is_directory(path))
        {
            return json_response(404, {{"error", "Directory not found"}});
        }

        json::array files;
        for (const auto& entry : fs::directory_iterator(path))
        {
            json::object file;
            file["name"] = entry.path().filename().string();
            file["size"] = static_cast<int64_t>(fs::file_size(entry));
            file["type"] = fs::is_directory(entry.status()) ? "directory" : "file";
            file["modified"] =
                std::chrono::duration_cast<std::chrono::seconds>(fs::last_write_time(entry).time_since_epoch()).count();

            files.push_back(file);
        }

        json::object response;
        response["path"] = fs::relative(path, ftp_root_).string();
        response["files"] = files;

        return json_response(200, response);
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
        return json_response(500, {{"error", e.what()}});
    }
}

response_t WebGuiFtpServer::handle_upload(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        // Parse multipart form data
        auto ct{req.at(http::field::content_type)};
        if (ct.find("multipart/form-data") == std::string::npos)
        {
            return json_response(400, {{"error", "Invalid content type"}});
        }

        // In real implementation, you would parse multipart data
        // This is a simplified version
        auto params{parse_query(req.body())};
        std::string path{params["path"]};
        std::string filename{params["filename"]};

        if (filename.empty())
        {
            return json_response(400, {{"error", "No filename specified"}});
        }

        fs::path filepath{fs::path(ftp_root_) / path / filename};

        // In a real implementation, you would save the file content
        // For now, create an empty file
        std::ofstream(filepath.string()).close();

        return json_response(200, {{"message", "File uploaded successfully"}, {"filename", filename}});
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
        return json_response(500, {{"error", e.what()}});
    }
}

// response_t WebGuiFtpServer::handle_download(const http::request<http::string_body>& req)
// {
//     logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

//     try
//     {
//         std::string filename{req.target().substr(strlen("/api/download/"))};

//         // URL decode filename
//         filename = url_decode(filename);

//         fs::path filepath{fs::path(ftp_root_) / filename};

//         if (!fs::exists(filepath) || fs::is_directory(filepath))
//         {
//             return json_response(404, {{"error", "File not found"}});
//         }

//         std::ifstream file(filepath.string(), std::ios::binary);
//         if (!file)
//         {
//             return json_response(500, {{"error", "Cannot open file"}});
//         }

//         http::response<http::file_body> res{http::status::ok, req.version()};
//         res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
//         res.set(http::field::content_type, "application/octet-stream");
//         res.set(http::field::content_disposition, "attachment; filename=\"" + filepath.filename().string() + "\"");
//         res.set(http::field::access_control_allow_origin, "*");

//         beast::error_code ec;
//         res.body().open(filepath.string().c_str(), beast::file_mode::read, ec);
//         if (ec)
//         {
//             logger_.Log(std::format("Error in {}: Cannot read file {}", __FUNCTION__, filepath.string()),
//                         Severities::Error);
//             return json_response(500, {{"error", "Cannot read file"}});
//         }

//         return res;

//         // res.prepare_payload();

//         // http::async_write(socket_,
//         //                   res,
//         //                   [self = shared_from_this()](beast::error_code ec, std::size_t)
//         //                   { self->socket_.shutdown(tcp::socket::shutdown_send, ec); });
//     }
//     catch (const std::exception& e)
//     {
//         logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
//         return json_response(500, {{"error", e.what()}});
//     }
// }

response_t WebGuiFtpServer::handle_delete(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        std::string filepath{req.target().substr(strlen("/api/delete/"))};
        filepath = url_decode(filepath);

        fs::path full_path{fs::path(ftp_root_) / filepath};

        if (!fs::exists(full_path))
        {
            return json_response(404, {{"error", "File not found"}});
        }

        bool success{false};
        if (fs::is_directory(full_path))
        {
            success = fs::remove_all(full_path) > 0;
        }
        else
        {
            success = fs::remove(full_path);
        }

        if (success)
        {
            return json_response(200, {{"message", "Deleted successfully"}});
        }
        else
        {
            return json_response(500, {{"error", "Failed to delete"}});
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
        return json_response(500, {{"error", e.what()}});
    }
}

response_t WebGuiFtpServer::handle_mkdir(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        auto params{parse_query(req.target())};
        std::string path{params["path"]};
        std::string dirname{params["name"]};

        if (dirname.empty())
        {
            return json_response(400, {{"error", "Directory name required"}});
        }

        fs::path full_path{fs::path(ftp_root_) / path / dirname};

        if (fs::exists(full_path))
        {
            return json_response(400, {{"error", "Directory already exists"}});
        }

        if (fs::create_directory(full_path))
        {
            return json_response(200, {{"message", "Directory created"}});
        }
        else
        {
            return json_response(500, {{"error", "Failed to create directory"}});
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
        return json_response(500, {{"error", e.what()}});
    }
}

response_t WebGuiFtpServer::handle_rmdir(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    return handle_delete(req);  // Same implementation
}

response_t WebGuiFtpServer::handle_rename(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        auto params{parse_query(req.target())};
        std::string path{params["path"]};
        std::string old_name{params["old"]};
        std::string new_name{params["new"]};

        if (old_name.empty() || new_name.empty())
        {
            return json_response(400, {{"error", "Both names required"}});
        }

        fs::path old_path{fs::path(ftp_root_) / path / old_name};
        fs::path new_path{fs::path(ftp_root_) / path / new_name};

        if (!fs::exists(old_path))
        {
            return json_response(404, {{"error", "File not found"}});
        }

        if (fs::exists(new_path))
        {
            return json_response(400, {{"error", "Target already exists"}});
        }

        fs::rename(old_path, new_path);
        return json_response(200, {{"message", "Renamed successfully"}});
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
        return json_response(500, {{"error", e.what()}});
    }
}

response_t WebGuiFtpServer::serve_static_file(const http::request<http::string_body>& req)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    try
    {
        std::string path{req.target()};
        if (path == "/")
            path = "/index.html";

        // Serve from embedded HTML or from filesystem
        if (path == "/index.html")
        {
            return html_response();
        }
        else
        {
            return json_response(404, {{"error", "Not found"}});
        }
    }
    catch (const std::exception& e)
    {
        logger_.Log(std::format("Error in {}: {}", __FUNCTION__, e.what()), Severities::Error);
        return json_response(500, {{"error", e.what()}});
    }
}

response_t WebGuiFtpServer::html_response()
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    http::response<http::string_body> res{http::status::ok, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.set(http::field::access_control_allow_origin, "*");
    res.body() = index_html_file_;

    return res;
}

response_t WebGuiFtpServer::json_response(int status, const json::object& body)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    response_t res{static_cast<http::status>(status), 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.set(http::field::access_control_allow_origin, "*");
    res.body() = json::serialize(body);

    return res;
}

response_t WebGuiFtpServer::cors_response()
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    response_t res{http::status::ok, 11};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::access_control_allow_methods, "GET, POST, PUT, DELETE, OPTIONS");
    res.set(http::field::access_control_allow_headers, "Content-Type");

    return res;
}

std::map<std::string, std::string> WebGuiFtpServer::parse_query(const std::string& query)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    std::map<std::string, std::string> params;
    std::istringstream iss(query);
    std::string pair;

    std::size_t qmark{query.find('?')};
    std::string query_string{(qmark != std::string::npos) ? query.substr(qmark + 1) : query};

    std::istringstream qiss(query_string);
    while (std::getline(qiss, pair, '&'))
    {
        std::size_t eq{pair.find('=')};
        if (eq != std::string::npos)
        {
            std::string key{pair.substr(0, eq)};
            std::string value{pair.substr(eq + 1)};
            params[url_decode(key)] = url_decode(value);
        }
    }

    return params;
}

std::string WebGuiFtpServer::url_decode(const std::string& str)
{
    logger_.Log(std::format("{}", __FUNCTION__), Severities::Trace);

    std::string result;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '%' && i + 2 < str.length())
        {
            int value;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> value)
            {
                result += static_cast<char>(value);
                i += 2;
            }
            else
            {
                result += str[i];
            }
        }
        else if (str[i] == '+')
        {
            result += ' ';
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}
