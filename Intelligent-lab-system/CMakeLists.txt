cmake_minimum_required(VERSION 3.5)

project(Intelligent-lab-system LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 设置 Qt5 自动处理 UI 文件、MOC 文件和资源文件
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置交叉编译器
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
SET(CMAKE_C_COMPILER "arm-linux-gcc")
SET(CMAKE_CXX_COMPILER "arm-linux-g++")

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/lzh/qt-everywhere-src-5.12.9/arm-qt/lib/cmake")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

# 查找 OpenCV 库
set(OpenCV_DIR /home/lzh/deep_leaning/opencv/build/install/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)
include_directories(/home/lzh/deep_leaning/opencv/build/install/include)

# 查找 ncnn 库
set(NCNN_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}//lib/ncnn)
set(NCNN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/ncnn)
include_directories(${NCNN_INCLUDE_DIR})
link_directories(${NCNN_LIB_DIR})

#查找openmp
set(CUSTOM_OPENMP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}//lib/openmp)

# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/arcface/*.h)
#源文件
file(GLOB ARC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/arcface/*.cpp)

if(ANDROID)
  add_library(Intelligent-lab-system SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${ARC_SOURCE_FILES}
  )
else()
  add_executable(Intelligent-lab-system
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${ARC_SOURCE_FILES}
  )
endif()

# 链接 Qt5 库
target_link_libraries(Intelligent-lab-system PRIVATE Qt5::Widgets ${OpenCV_LIBS} ncnn pthread ${CUSTOM_OPENMP_LIB_DIR}/libgomp.a)


