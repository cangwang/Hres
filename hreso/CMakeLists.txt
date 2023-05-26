# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)
# DVK_USE_PLATFORM_ANDROID_KHR 一些Android vulkan的特殊接口
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DVK_USE_PLATFORM_ANDROID_KHR -DVK_NO_PROTOTYPES")
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

#include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${ANDROID_NDK}/sources/third_party/shaderc/include)

add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
        ${ANDROID_NDK}/sources/third_party/shaderc/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)


include_directories(${CMAKE_SOURCE_DIR} src/main/cpp)
add_library( # Sets the name of the library.
        hres

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/hresjni.cpp
        src/main/cpp/egl/eglcore.cpp
        src/main/cpp/bean/imageoptionparams.cpp
        src/main/cpp/bean/engineoption.cpp
        src/main/cpp/transform/hrestransformer.cpp
        src/main/cpp/transform/imagehrestransformer.cpp
        src/main/cpp/transform/ImageVulkanHresTransformer.cpp
        src/main/cpp/transform/optionparser.cpp
        src/main/cpp/transform/listenermanager.cpp
        src/main/cpp/filter/hqxfilter.cpp
        src/main/cpp/filter/simplefilter.cpp
        src/main/cpp/filter/filtercontroller.cpp
        src/main/cpp/filter/filterlistener.cpp
        src/main/cpp/filter/fbfilter.cpp
        src/main/cpp/filter/fsrfilter.cpp
        src/main/cpp/filter/fsrupfilter.cpp
        src/main/cpp/util/shaderutil.cpp
        src/main/cpp/util/glfloatarray.cpp
        src/main/cpp/util/loadtextureutil.cpp
        src/main/cpp/util/texcoordsutil.cpp
        src/main/cpp/util/vertexutil.cpp
        src/main/cpp/vulkan_wrapper/vulkan_wrapper.cpp
        src/main/cpp/vulkan/VideoRenderContext.cpp
        src/main/cpp/vulkan/VideoRenderer.cpp
        src/main/cpp/vulkan/VKUtils.cpp
        src/main/cpp/vulkan/engine/VKBufferManager.cpp
        src/main/cpp/vulkan/engine/VKDeviceManager.cpp
        src/main/cpp/vulkan/engine/VKEngineRenderer.cpp
        src/main/cpp/vulkan/engine/VKOffScreen.cpp
        src/main/cpp/vulkan/engine/VKRender.cpp
        src/main/cpp/vulkan/engine/VKSwapChainManager.cpp
        src/main/cpp/vulkan/engine/VKTextureManager.cpp
        src/main/cpp/vulkan/filter/VulkanFilter.cpp
        src/main/cpp/vulkan/filter/FilterUtil.cpp
        src/main/cpp/vulkan/filter/OffScreenFilter.cpp
        src/main/cpp/vulkan/filter/OffScreenImageFilter.cpp
        src/main/cpp/vulkan/filter/FsrVulkanFilter.cpp
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
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       shaderc_lib)

