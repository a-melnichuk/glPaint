#include "engine.h"


struct android_app;
Engine engine;

extern "C" {

    JNIEXPORT void JNICALL
    Java_melnichuk_al_coloringbookapp_ui_ColoringBookNativeActivity_clear(JNIEnv *env, jclass type) {

       // ndk_helper::JNIHelper* helper = ndk_helper::JNIHelper::GetInstance();
        //JNIEnv* e = helper->AttachCurrentThread();
        engine.quad.clearPaint();
       // helper->DetachCurrentThread();
        //engine.clearPaint();
    }

    JNIEXPORT void JNICALL
    Java_melnichuk_al_coloringbookapp_ui_ColoringBookNativeActivity_setPaint(JNIEnv *env, jclass type,
                                                                             jint color) {
        int32_t col = (int32_t) color;
        engine.quad.setPaint(col);
    }

}

void android_main(android_app* state) {
    app_dummy();

    engine.setState(state);

    // Init helper functions
    state->userData = &engine;
    state->onAppCmd = Engine::handleCmd;
    state->onInputEvent = Engine::handleInput;

#ifdef USE_NDK_PROFILER
    monstartup("libTeapotNativeActivity.so");
#endif

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.

        int events;
        android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while (ALooper_pollAll(engine.isReady() ? 0 : -1, NULL, &events, (void**)&source) >= 0) {
            // Process this event.
            if (source != NULL)
                source->process(state, source);

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine.termDisplay();
                return;
            }
        }

        if (engine.isReady()) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine.drawFrame();
        }
    }
}