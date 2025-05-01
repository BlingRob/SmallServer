project("Server")

set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_BUILD_TYPE "Debug")

# add_compile_definitions(
#     $<$<COMPILE_LANG_AND_ID:CXX,Clang>:LLVM_ENABLE_RUNTIMES=libunwind>
#     $<$<COMPILE_LANG_AND_ID:CXX,Clang>:LIBCXX_USE_COMPILER_RT=YES>
#     $<$<COMPILE_LANG_AND_ID:CXX,Clang>:LIBCXXABI_USE_COMPILER_RT=YES>
#     $<$<COMPILE_LANG_AND_ID:CXX,Clang>:LIBUNWIND_USE_COMPILER_RT=YES>
#     )

file(COPY ${CMAKE_SOURCE_DIR}/Defaults/hello.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ${CMAKE_SOURCE_DIR}/Defaults/default.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# set(CPP_FILES main.cpp client_request.cpp server.cpp client_request.cpp parser.cpp cmd_interpreter.cpp utils.cpp server_console_commander.cpp) # logger.cpp

set(CPP_FILES main.cpp http_server/server.cpp)

add_executable (${PROJECT_NAME} ${CPP_FILES})

find_package(Boost REQUIRED)
if(Boost_FOUND)
  # include_directories(${Boost_INCLUDE_DIRS})
  # link_directories(${Boost_LIBRARY_DIR})
  target_link_libraries (${PROJECT_NAME} boost::boost)
endif()


