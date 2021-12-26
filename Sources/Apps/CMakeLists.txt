cmake_minimum_required(VERSION 3.14)

project(qt-qml-ai VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick Multimedia REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)


find_library(onnxruntime_LIBS 
  NAMES onnxruntime 
  PATHS ../../Libs/onnxruntime-linux-x64-1.10.0/lib
)
set(onnxruntime_INCLUDE_DIRS
  ../../Libs/onnxruntime-linux-x64-1.10.0/include
)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
  Source/Barcode
  Source/EntryPoint
  Source/Face
  Source/Hand
  Source/OrtNet
  ${OpenCV_INCLUDE_DIRS}
  ${onnxruntime_INCLUDE_DIRS}

    # eRPC App includes
    ../External/liberpc-app/erpc_service

    # eRPC Ext includes
    ../External/liberpc-app/liberpc_ext/setup
    ../External/liberpc-app/liberpc_ext/transports
    # Main App includes
    Source/Nucleo

    Source/Todo
)

target_sources(${PROJECT_NAME} PRIVATE
  Source/EntryPoint/main.cpp
  Source/Barcode/barcode.h
  Source/Face/face.cpp
  Source/Hand/hand.cpp
  Source/OrtNet/OrtNet.cpp
  Resource/qml.qrc

  # eRPC APP sources
  ../External/liberpc-app/erpc_service/erpc_nucleo_uart_client.cpp
  Source/Nucleo/nucleo.cpp

  # eRPC Ext sources
  ../External/liberpc-app/liberpc_ext/setup/erpc_setup_serial.cpp
  ../External/liberpc-app/liberpc_ext/transports/erpc_serial_transport.cpp

  # Shooping Model
  Source/Todo/dbhelper.cpp
  Source/Todo/todo.cpp
  Source/Todo/todomodel.cpp
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt5::Core
  Qt5::Quick
  Qt5::Multimedia
  Qt5::Sql
  ZXing::ZXing
  ${OpenCV_LIBS}
  ${onnxruntime_LIBS} 
  erpc
  ${CMAKE_THREAD_LIBS_INIT}
)