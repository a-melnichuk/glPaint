add_library(ndk_helper STATIC
            gestureDetector.cpp
            gl3stub.cpp
            GLContext.cpp
            interpolator.cpp
            JNIHelper.cpp
            perfMonitor.cpp
            sensorManager.cpp
            shader.cpp
            tapCamera.cpp
            vecmath.cpp)

target_include_directories(ndk_helper PRIVATE
                           ${ANDROID_NDK}/sources/android/native_app_glue)
