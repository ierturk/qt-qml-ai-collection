cmake_minimum_required(VERSION 3.0.0)

# set the project name
project(qt-qml-ai VERSION 1.0)

add_subdirectory(Sources/External/liberpc-app/liberpc_ext/CMake)
add_subdirectory(Sources/External/libzxing-cpp)
add_subdirectory(Sources/Apps)
add_subdirectory(Sources/External/swc)