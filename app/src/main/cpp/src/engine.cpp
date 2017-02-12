//
// Created by al on 11.02.17.
//

#include "engine.h"


Engine::Engine() :
        initializedResources(false),
        hasFocus(false),
        app(NULL)
{
    glContext = ndk_helper::GLContext::GetInstance();
}

Engine::~Engine()
{

}

void Engine::loadResources()
{
    quad.setViewportDimens(glContext->GetScreenWidth(), glContext->GetScreenHeight());
    quad.init();
}

void Engine::cleanup()
{
    quad.cleanup();
}

int Engine::initDisplay()
{

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Note that screen size might have been changed
    glViewport(0, 0, glContext->GetScreenWidth(), glContext->GetScreenHeight());

    if (!initializedResources) {
        glContext->Init(app->window);
        loadResources();
        initializedResources = true;
    } else {
        // initialize OpenGL ES and EGL
        if (EGL_SUCCESS != glContext->Resume(app->window)) {
            cleanup();
            loadResources();
        }
    }

    showUI();

    return 0;
}

void Engine::drawFrame()
{
    float fps;
    if (monitor.Update(fps)) {
        updateFPS(fps);
    }

    // renderer_.Update(monitor.GetCurrentTime());
    quad.update((float) monitor.GetCurrentTime(), touching, touchPosClip);
    // Just fill the screen with a color.
    glClearColor(.5f, .5f, .5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quad.draw();

    // Swap
    if (EGL_SUCCESS != glContext->Swap()) {
        cleanup();
        loadResources();
    }
}

void Engine::termDisplay()
{
    glContext->Suspend();
}

void Engine::trimMemory()
{
    LOGI("Trimming memory");
    glContext->Invalidate();
}

int32_t Engine::handleInput(android_app *app, AInputEvent *event)
{
    Engine* eng = (Engine*)app->userData;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

        unsigned int flags =  AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

        switch (flags) {
            case AMOTION_EVENT_ACTION_UP:
                eng->setTouching(false);
                LOGI("UP");
                break;
            case AMOTION_EVENT_ACTION_DOWN:
                eng->setTouching(true);
                LOGI("DOWN");
                break;
        }



        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);

        glm::vec2 touchPos(x, y);
        eng->setTouchPos(touchPos);
        //eng->quad.update((float) eng->monitor.GetCurrentTime(), touching, eng->touchPosClip);
        //eng->quad.draw();
        return 1;
    }
    return 0;
}

void Engine::handleCmd(struct android_app *app, int32_t cmd)
{
    Engine* eng = (Engine*) app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (app->window != NULL) {
                eng->initDisplay();
                eng->drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            eng->termDisplay();
            eng->hasFocus = false;
            break;
        case APP_CMD_STOP:
            break;
        case APP_CMD_GAINED_FOCUS:

            // Start animation
            eng->hasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            // Also stop animating.
            eng->hasFocus = false;
            eng->drawFrame();
            break;
        case APP_CMD_LOW_MEMORY:
            // Free up GL resources
            eng->trimMemory();
            break;
    }
}

void Engine::setState(android_app *state)
{
    app = state;
    ndk_helper::JNIHelper::Init(state->activity, HELPER_CLASS_NAME);
}

bool Engine::isReady()
{
    return hasFocus;
}

void Engine::showUI()
{
    JNIEnv* jni;
    app->activity->vm->AttachCurrentThread(&jni, NULL);

    // Default class retrieval
    jclass clazz = jni->GetObjectClass(app->activity->clazz);
    jmethodID methodID = jni->GetMethodID(clazz, "showUI", "()V");
    jni->CallVoidMethod(app->activity->clazz, methodID);

    app->activity->vm->DetachCurrentThread();
    return;
}

void Engine::updateFPS(float fFPS)
{
    JNIEnv* jni;
    app->activity->vm->AttachCurrentThread(&jni, NULL);

    // Default class retrieval
    jclass clazz = jni->GetObjectClass(app->activity->clazz);
    jmethodID methodID = jni->GetMethodID(clazz, "updateFPS", "(F)V");
    jni->CallVoidMethod(app->activity->clazz, methodID, fFPS);

    app->activity->vm->DetachCurrentThread();
    return;
}

void Engine::setTouching(bool touching) {
    this->touching = touching;
}

bool Engine::isTouching() {
    return touching;
}

void Engine::setTouchPos(glm::vec2 pos)
{
    touchPos = pos;
    int32_t w = glContext->GetScreenWidth();
    int32_t h = glContext->GetScreenHeight();

    touchPosClip.x = 2 * touchPos.x / w - 1;
    touchPosClip.y = 1 - 2 * touchPos.y / h;
    LOGI("TOUCH x: %f y: %f", touchPosClip.x, touchPosClip.y);
}






