//
// Created by al on 11.02.17.
//
#include "quad.h"


static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
};

static const GLfloat texCoordFlipped[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
};

static const GLfloat texCoord[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
};


Quad::Quad():
        viewportWidth(0),
        viewportHeight(0),
        textureWidth(0),
        textureHeight(0),
        paint(glm::vec3(1.0, 0.0, 0.0))
{

}

Quad::~Quad()
{
    cleanup();
}

void Quad::cleanup()
{

}

void Quad::init()
{

   // glFrontFace(GL_CCW);

    initBuffers();

    ndk_helper::JNIHelper* helper = ndk_helper::JNIHelper::GetInstance();
    bool hasAlpha = false;
    texture = helper->LoadTexture("texture/bear.jpg", &textureWidth, &textureHeight, &hasAlpha);
    initTextureFramebuffer(&maskFramebuffer, &maskTexture, textureWidth, textureHeight);
    initTextureFramebuffer(&srcFramebuffer, &srcTexture, textureWidth, textureHeight);
    initTextureFramebuffer(&dstFramebuffer, &dstTexture, textureWidth, textureHeight);

    initShaders(&dstProgram, "shader/dst.vert", "shader/dst.frag");
    dstMask = glGetUniformLocation(dstProgram, "u_Mask");
    dstSampler = glGetUniformLocation(dstProgram, "u_Sampler");
    dstTouchHandle = glGetUniformLocation(dstProgram, "u_Touch");
    dstPaintHandle = glGetUniformLocation(dstProgram, "u_Color");

    initShaders(&maskProgram, "shader/mask.vert", "shader/mask.frag");
    maskSampler = glGetUniformLocation(maskProgram, "u_Sampler");

    initShaders(&program, "shader/main.vert", "shader/main.frag");
    sampler = glGetUniformLocation(program, "u_Sampler");
    touchHandle = glGetUniformLocation(program, "u_Touch");

    drawMask();
    drawSrcInitial();
}

void Quad::setViewportDimens(int32_t w, int32_t h) {
    viewportWidth = w;
    viewportHeight = h;
}

bool Quad::initTextureFramebuffer(GLuint* outFramebuffer, GLuint* outTexture, int32_t width, int32_t height) {
    GLuint texture, framebuffer;

    glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &texture);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGI("ERROR: FRAMEBUFFER INCOMPLETE");
        return false;
    }

    LOGI("SUCCESS: FRAMEBUFFER COMPLETE");

    *outFramebuffer = framebuffer;
    *outTexture = texture;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Quad::initShaders(GLuint* outProgram, const char* vertexPath, const char* fragmentPath)
{
    GLuint vertShader, fragShader;
    GLuint program = glCreateProgram();
    if (!ndk_helper::shader::CompileShader(&vertShader, GL_VERTEX_SHADER, vertexPath)) {
        LOGI("Failed to compile vertex shader");
        glDeleteProgram(program);
        return;
    }

    if (!ndk_helper::shader::CompileShader(&fragShader, GL_FRAGMENT_SHADER, fragmentPath)) {
        LOGI("Failed to compile fragment shader");
        glDeleteProgram(program);
        return;
    }

    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    glBindAttribLocation(program, ATTR_VERTEX, "a_Pos");
    glBindAttribLocation(program, ATTR_UV, "a_TexCoord");

    if (!ndk_helper::shader::LinkProgram(program)) {
        LOGI("Failed to link program: %d", program);
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
            glDeleteProgram(program);
        return;
    }

    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);

    *outProgram = program;
}

void Quad::initBuffers()
{
    GLuint indices[] = {
            0, 3, 2, // First Triangle
            0, 3, 1  // Second Triangle
    };
    numIndices = sizeof(indices) / sizeof(indices[0]);
    LOGI("NUM INDICES %d", numIndices);
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    GLfloat vertices[] = {
            -1.0f, -1.0f,   1.0f, 1.0f,
             1.0f, -1.0f,   0.0f, 1.0f,
            -1.0f,  1.0f,   1.0f, 0.0f,
             1.0f,  1.0f,   0.0f, 0.0f,
    };

    int32_t stride = 4 * sizeof(GLfloat);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * stride, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::drawMask()
{
    glViewport(0, 0, textureWidth, textureHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, maskFramebuffer);
    glUseProgram(maskProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(maskSampler, 0);

    glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTR_VERTEX);
    glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, 0, 0, texCoordFlipped);
    glEnableVertexAttribArray(ATTR_UV);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
}

void Quad::drawSrcInitial()
{
    glViewport(0, 0, textureWidth, textureHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, srcFramebuffer);

    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(maskProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(maskSampler, 0);

    glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTR_VERTEX);
    glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, 0, 0, texCoordFlipped);
    glEnableVertexAttribArray(ATTR_UV);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
}

void Quad::drawDst()
{
    glViewport(0, 0, textureWidth, textureHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, dstFramebuffer);

    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(dstProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glUniform1i(dstMask, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glUniform1i(dstSampler, 1);

    glUniform2f(dstTouchHandle, touchPosClip.x, touchPosClip.y);
    glUniform3f(dstPaintHandle, paint.r, paint.g, paint.b);

    glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTR_VERTEX);
    glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, 0, 0, texCoord);
    glEnableVertexAttribArray(ATTR_UV);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);
}

void Quad::draw2()
{
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    glUniform1i(sampler, 0);

    glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTR_VERTEX);
    glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, 0, 0, texCoord);
    glEnableVertexAttribArray(ATTR_UV);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Quad::draw()
{
    if (clear) {
        drawSrcInitial();
        clear = false;
    }

    if (isTouching)
        drawDst();

    draw2();

    if (isTouching)
        swapOutputTextures();
}

void Quad::swapOutputTextures()
{
    GLuint tmpFramebuffer = dstFramebuffer;
    GLuint tmpTexture = dstTexture;

    dstFramebuffer = srcFramebuffer;
    dstTexture = srcTexture;

    srcFramebuffer = tmpFramebuffer;
    srcTexture = tmpTexture;
}

void Quad::draw1()
{
    /*
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    int32_t stride = 4 * sizeof(GLfloat);
    // Pass the vertex data
    glVertexAttribPointer(ATTR_VERTEX, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(ATTR_VERTEX);

    glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(ATTR_UV);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(sampler, 0);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

     glDisableVertexAttribArray(ATTR_VERTEX);
    glDisableVertexAttribArray(ATTR_UV);

    */
}

void Quad::update(float time, bool isTouching, glm::vec2 touchPosClip)
{
    float x = touchPosClip.x - this->touchPosClip.x;
    float y = touchPosClip.y - this->touchPosClip.y;
    float d = sqrtf(x * x + y * y);
    float r = 0.1f;
    float dt = .25f * r;

    if (d > dt) {
        int numDraws = (int) (d / dt);
        glm::vec2 oldTouchPos = this->touchPosClip;
        glm::vec2 v;

        for (int i = 0; i < numDraws; ++i) {
            float t = (float) i / numDraws;
            v.x = (1.f - t) * oldTouchPos.x + t * touchPosClip.x;
            v.y = (1.f - t) * oldTouchPos.y + t * touchPosClip.y;
            this->touchPosClip = v;
            draw();
        }
    }


    this->isTouching = isTouching;
    this->touchPosClip = touchPosClip;
}

void Quad::setPaint(int32_t color)
{
    int r = (color >> 16) & 0x000000ff;
    int g = (color >> 8)  & 0x000000ff;
    int b =  color        & 0x000000ff;

    paint.r = (float) r / 255;
    paint.g = (float) g / 255;
    paint.b = (float) b / 255;
}

void Quad::clearPaint()
{
    clear = true;
}






