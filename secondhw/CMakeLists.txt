cmake_minimum_required(VERSION 3.13.4)
project(main.out)
set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/include")

link_directories("/usr/lib")
link_directories("/usr/local/lib")

add_executable(${PROJECT_NAME} main.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} cryptopp)

