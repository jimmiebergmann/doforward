cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project (doforward_server_client)
enable_language(C CXX)

# Check for c++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CXX_COMPILER_FLAGS} -std=c++11")
	message(STATUS "Supports -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CXX_COMPILER_FLAGS} -std=c++0x")
	message(STATUS "Supports -std=c++0x")
else()
	message(STATUS "The compiler ${CXX_COMPILER_FLAGS} has no C++11 support. Please use a different C++ compiler.")
endif()

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Sources
set(SERVER_MAIN			source/mains/ServerApp.cpp)
set(SERVER_SOURCES		source/server/Balancer.cpp)
set(SERVER_TESTS_SOURCES	test/server/ServerTests.cpp)


# Tests

# Tests - Server
add_executable(server_tests ${SERVER_SOURCES} ${SERVER_TESTS_SOURCES})
target_link_libraries(		server_tests PUBLIC gcov)
target_compile_definitions(	server_tests PUBLIC ${CXX_COMPILER_FLAGS})
target_include_directories(	server_tests PUBLIC include/server include/common)
target_compile_options(		server_tests PUBLIC --coverage )

# Server
add_executable(doforward ${SERVER_SOURCES} ${SERVER_MAIN})
target_compile_definitions(	doforward PUBLIC ${CXX_COMPILER_FLAGS})
target_include_directories(	doforward PUBLIC include/server include/common)


