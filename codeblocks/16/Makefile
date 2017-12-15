mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))

# Create required directories
CreateDirectory:
	cd $(current_dir)/../../; mkdir -p build

# CMake build script
Build: CreateDirectory
	cd $(current_dir)/../../build; cmake ..; make


# Server tests
Debug_common_tests: Release_common_tests

Release_common_tests: Build
#	.$(current_dir)/../../bin/server_tests


# Node
Debug_node: Release_node

Release_node: Build


# Server
Debug_server: Release_server

Release_server: Build


# Server tests
Debug_server_tests: Release_server_tests

Release_server_tests: Build
	.$(current_dir)/../../bin/server_tests

