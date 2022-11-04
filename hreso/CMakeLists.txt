# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/jniLibs/${ANDROID_ABI})
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 去除未使用函数与变量
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -Os -Wall -flto")
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -O3 -flto")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -Wall -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -Wall -s")

include_directories(${CMAKE_SOURCE_DIR} src/main/cpp)
add_library( # Sets the name of the library.
        hres

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/hresjni.cpp
        src/main/cpp/egl/eglcore.cpp
        src/main/cpp/transform/imageoptionparams.cpp
        src/main/cpp/transform/hrestransformer.cpp
        src/main/cpp/transform/imagehrestransformer.cpp
        src/main/cpp/transform/optionparser.cpp
        src/main/cpp/transform/listenermanager.cpp
        src/main/cpp/filter/hqxfilter.cpp
        src/main/cpp/filter/simplefilter.cpp
        src/main/cpp/filter/filtercontroller.cpp
        src/main/cpp/filter/filterlistener.cpp
        src/main/cpp/filter/fbfilter.cpp
        src/main/cpp/filter/fsrfilter.cpp
        src/main/cpp/util/shaderutil.cpp
        src/main/cpp/util/glfloatarray.cpp
        src/main/cpp/util/loadtextureutil.cpp
        src/main/cpp/util/texcoordsutil.cpp
        src/main/cpp/util/vertexutil.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

add_library(parson
        STATIC
        src/main/cpp/util/parson.c)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


include_directories(libs/arm64-v8a/include)
set(DIR ../../../../libs/arm64-v8a/lib)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libavcodec.so)

add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libavdevice.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libavformat.so)

add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libavutil.so)

add_library(postproc
        SHARED
        IMPORTED)
set_target_properties(postproc
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libpostproc.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libswresample.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libswscale.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${DIR}/libavfilter.so)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

# 设置去除未使用代码的链接flag
#SET_TARGET_PROPERTIES(yyeva2 PROPERTIES LINK_FLAGS "-Wl,-s,--gc-sections")

target_link_libraries( # Specifies the target library.
        hres
        android
        GLESv3
        EGL
        parson
        avfilter
        avcodec
        avdevice
        avformat
        avutil
        postproc
        swresample
        swscale

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib} )

