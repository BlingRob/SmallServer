project("Server")

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED TRUE)

find_package(tomlplusplus REQUIRED CONFIG)
find_package(Boost REQUIRED)

set(CMAKE_BUILD_TYPE "Debug")

file(COPY ${CMAKE_SOURCE_DIR}/Defaults/ftp_index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_SOURCE_DIR}/Defaults/cfg.toml DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CPP_FILES main.cpp 
              base_http_server/server.cpp 
              ftp_server/server.cpp 
              ftp_server/session.cpp 
              ftp_server/web_gui_server.cpp 
              logger_subsystem/boost_logger_impl.cpp 
              utils.cpp)

add_executable (${PROJECT_NAME} ${CPP_FILES})

if(Boost_FOUND)
  target_link_libraries (${PROJECT_NAME} boost::boost tomlplusplus::tomlplusplus)
endif()


