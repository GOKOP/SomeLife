cmake_minimum_required(VERSION 3.20)
project(SomeLife)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(OpenCL REQUIRED)

include_directories(src)
file(GLOB SOURCES "src/*.cpp")

add_executable(somelife ${SOURCES})
set_target_properties(somelife PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED 20)
target_link_libraries(somelife sfml-graphics)
target_link_libraries(somelife OpenCL::OpenCL)

if(NOT MSVC)
	target_compile_options(somelife PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(
	TARGET somelife POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/res
	${CMAKE_CURRENT_BINARY_DIR}/res)

add_custom_command(
	TARGET somelife POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/recipes
	${CMAKE_CURRENT_BINARY_DIR}/recipes)

add_custom_target(do_always ALL)

add_custom_command(
	TARGET do_always POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_SOURCE_DIR}/src/simulation.cl
	${CMAKE_CURRENT_BINARY_DIR}/simulation.cl)
