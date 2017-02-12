//
// Created by al on 11.02.17.
//

#ifndef COLORINGBOOKAPP_ENGINE_H
#define COLORINGBOOKAPP_ENGINE_H

#include <jni.h>
#include <errno.h>
#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <cpu-features.h>
#include <glm/vec2.hpp>

#include "NDKHelper.h"
#include "JNIHelper.h"
#include "quad.h"

#define HELPER_CLASS_NAME "melnichuk/al/coloringbookapp/utils/NDKHelper"

class Engine {
    ndk_helper::GLContext* glContext;

    bool initializedResources;
    bool hasFocus;
    bool touching;


    android_app* app;


    void updateFPS(float fFPS);
    void showUI();
public:
    static void handleCmd(struct android_app *app, int32_t cmd);
    static int32_t handleInput(android_app *app, AInputEvent *event);

    Engine();
    ~Engine();
    void setState(android_app *state);

    void setTouchPos(glm::vec2 pos);
    int initDisplay();
    void loadResources();
    void cleanup();
    void drawFrame();
    void termDisplay();
    void trimMemory();
    bool isReady();
    bool isTouching();
    void setTouching(bool touching);

    ndk_helper::PerfMonitor monitor;
    glm::vec2 touchPos, touchPosClip;
    Quad quad;
};
#endif //COLORINGBOOKAPP_ENGINE_H
