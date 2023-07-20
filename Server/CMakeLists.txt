project("SmallServer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_system")

find_package(Boost REQUIRED COMPONENTS system thread regex program_options)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIR})
endif()

add_executable (${PROJECT_NAME} "StepikHttpServer.cpp" "Headers.h" "Parser.h" "Server.h" "Server.cpp")

target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
