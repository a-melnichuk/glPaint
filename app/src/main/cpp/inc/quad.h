//
// Created by al on 11.02.17.
//

#ifndef COLORINGBOOKAPP_QUAD_H
#define COLORINGBOOKAPP_QUAD_H
#include <GLES/gl.h>
#include "NDKHelper.h"
#include <stdexcept>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <cmath>

class Quad {
public:
    Quad();
    ~Quad();

    enum ATTR {
        ATTR_VERTEX,
        ATTR_UV
    };

    struct Vertex {
        GLfloat pos[3];
        GLfloat textCoord[2];
    };

    void init();
    void initBuffers();
    void update(float dt, bool isTouching, glm::vec2 touchPosViewport);

    void draw();
    void draw1();
    void draw2();
    void cleanup();

    void clearPaint();
    void setPaint(int32_t color);

    void setViewportDimens(int32_t w, int32_t h);
private:
    int32_t viewportWidth, viewportHeight;
    int32_t textureWidth, textureHeight;

    uint32_t numIndices;
    bool isTouching;
    bool clear;
    glm::vec2 touchPosClip;
    glm::vec3 paint;
    GLuint VBO, IBO;
    GLuint touchHandle;
    GLuint maskProgram, maskFramebuffer, maskTexture, maskSampler;
    GLuint srcFramebuffer, srcTexture;
    GLuint dstProgram, dstFramebuffer, dstTexture, dstSampler, dstMask, dstTouchHandle, dstPaintHandle;
    GLuint program, sampler, texture;

    void initShaders(GLuint *outProgram, const char *vertexPath, const char *fragmentPath);

    bool initTextureFramebuffer(GLuint *outFramebuffer, GLuint *outTexture, int32_t width, int32_t height);

    void drawMask();


    void drawSrcInitial();

    void drawDst();

    void swapOutputTextures();
};

#endif //COLORINGBOOKAPP_QUAD_H
