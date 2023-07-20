project("final")
cmake_minimum_required (VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_system")
set(Boost_LIBRARY_DIR "/usr/lib/x86_64-linux-gnu")
find_package(Boost 1.54.0 REQUIRED COMPONENTS system thread regex program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIR})
endif()

add_executable (final "StepikHttpServer.cpp" "Headers.h" "Parser.h" "Server.h" "Server.cpp")

target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
