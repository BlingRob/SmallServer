project("Server")

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE "Debug")


file(COPY ${CMAKE_SOURCE_DIR}/Defaults/hello.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_SOURCE_DIR}/Defaults/default.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CPP_FILES main.cpp http_server/server.cpp utils.cpp)

add_executable (${PROJECT_NAME} ${CPP_FILES})

find_package(Boost REQUIRED)
if(Boost_FOUND)
  target_link_libraries (${PROJECT_NAME} boost::boost)
endif()


