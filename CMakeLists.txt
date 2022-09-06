cmake_minimum_required(VERSION 3.20)
project(SomeLife)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

include_directories(src)
file(GLOB SOURCES "src/*.cpp")

add_executable(somelife ${SOURCES})
set_target_properties(somelife PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED 17)
target_link_libraries(somelife sfml-graphics)
