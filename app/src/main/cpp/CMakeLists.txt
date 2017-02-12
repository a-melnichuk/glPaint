# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# build native_app_glue as a static lib
add_library(app-glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -fno-exceptions -fno-rtti")

# build the ndk-helper library

include_directories(lib inc src ./)

set(ndk_helper_dir ../lib/ndk_helper)
add_subdirectory(${ndk_helper_dir} ndk_helper)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.


add_library( # Sets the name of the library.
             color_book
             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/engine.cpp
             src/quad.cpp
             color_book.cpp )


target_include_directories(color_book PRIVATE
                           ${ANDROID_NDK}/sources/android/cpufeatures
                           ${ANDROID_NDK}/sources/android/native_app_glue
                           ${ndk_helper_dir})


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       color_book
                       android
                       app-glue
                       atomic
                       EGL
                       GLESv2
                       log
                       ndk_helper )
