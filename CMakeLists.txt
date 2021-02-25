cmake_minimum_required(VERSION 3.10)

project(Json_parser)

include_directories(include)

file(GLOB SOURCE "src/*.c")
add_executable(json_parser ${SOURCE})
