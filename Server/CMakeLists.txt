project("Core")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_system")

find_package(Boost REQUIRED COMPONENTS system thread regex program_options)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIR})
endif()

file(COPY ${CMAKE_SOURCE_DIR}/Defaults/hello.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_SOURCE_DIR}/Defaults/default.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

set(CPP_FILES main.cpp client_request.cpp server.cpp client_request.cpp parser.cpp cmd_interpreter.cpp utils.cpp server_console_commander.cpp logger.cpp)

add_executable (${PROJECT_NAME} ${CPP_FILES})

target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
