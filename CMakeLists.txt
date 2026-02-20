cmake_minimum_required(VERSION 3.15)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UTF-8 支持
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    elseif(MSVC)
	        add_compile_options(/utf-8)
	endif()

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

	file(GLOB_RECURSE SOURCES "src/*.cpp")
	file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

	add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
