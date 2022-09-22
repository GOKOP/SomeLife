cmake_minimum_required(VERSION 3.20)
project(SomeLife)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(OpenMP)

include_directories(src)
file(GLOB SOURCES "src/*.cpp")

add_executable(somelife ${SOURCES})
set_target_properties(somelife PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED 17)
target_link_libraries(somelife sfml-graphics)

if(OpenMP_CXX_FOUND)
	target_link_libraries(somelife OpenMP::OpenMP_CXX)
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
