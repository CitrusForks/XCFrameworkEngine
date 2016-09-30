/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#if defined(XCGRAPHICS_GL)
#include "XCGraphicsGL.h"
#include "Utils/GLShader.h"

XCGraphicsGL::XCGraphicsGL(void)
{
    m_clearColor = XCVec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_initDone = false;
}

XCGraphicsGL::~XCGraphicsGL(void)
{
}

void XCGraphicsGL::Destroy()
{
    m_XCShaderSystem->Destroy();
    m_renderingPool->Destroy();
}

void XCGraphicsGL::Init(HWND _mainWnd, i32 _width, i32 _height, bool _enable4xMsaa)
{
    m_hMainWnd = _mainWnd;
    m_ClientWidth = _width;
    m_ClientHeight = _height;
    m_Enable4xMsaa = _enable4xMsaa;

    SetupPipeline();
}

void XCGraphicsGL::SetupPipeline()
{
    //clockwise front face
    glFrontFace(GL_CW);

    //Culling enabled for back
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    SetupShadersAndRenderPool();

#if defined(DEBUG_GRAPHICS_PIPELINE)
    glGenVertexArrays(Max_VAOs, m_VAOs);
    glBindVertexArray(m_VAOs[VAO_Triangle]);

    std::vector<GLfloat> verts = {
        -0.90f, -0.90f,  -0.90f, 0.85f,  0.85f, -0.90f, //Tri 1
        0.90f, -0.85f,   -0.85f, 0.90f,  0.90f, 0.90f   //Tri 2
    };

    m_triangleVertices.assign(verts.begin(), verts.end());

    glGenBuffers(Max_Buffers, m_buffers);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, m_triangleVertices.size() * sizeof(GLfloat), &m_triangleVertices[0], GL_STATIC_DRAW);

    GLuint program = LoadShader("Assets/Shaders/Default/default_vs.glsl", "Assets/Shaders/Default/default_ps.glsl");
    glUseProgram(program);
    glVertexAttribPointer(VertexPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(VertexPosition);
#endif
}

void XCGraphicsGL::SetupShadersAndRenderPool()
{
    XCGraphics::SetupShadersAndRenderPool();
}

void XCGraphicsGL::SetupDevice()
{
}

void XCGraphicsGL::SetupSwapChain()
{
}

void XCGraphicsGL::SetupRenderTargets()
{
}

void XCGraphicsGL::SetupDepthStencilBuffer()
{
}

void XCGraphicsGL::SetupDepthStencilStates()
{
}

void XCGraphicsGL::SetupDepthView()
{
}

void XCGraphicsGL::SetupViewPort()
{
    XCGraphics::SetupViewPort();
}

void XCGraphicsGL::Update(f32 dt)
{
}

void XCGraphicsGL::BeginScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

#if defined(DEBUG_GRAPHICS_PIPELINE)
    glBindVertexArray(m_VAOs[VAO_Triangle]);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
#endif
}

void XCGraphicsGL::BeginSecondaryScene()
{
}

void XCGraphicsGL::EndSecondaryScene()
{
}

void XCGraphicsGL::EndScene()
{
    glFlush();
}

void XCGraphicsGL::TurnOnZ()
{
}

void XCGraphicsGL::TurnOffZ()
{
}

void XCGraphicsGL::SetLessEqualDepthStencilView(ID3DDeviceContext& context, bool turnOn)
{
}

void XCGraphicsGL::GoFullscreen(bool go)
{
}

void XCGraphicsGL::OnResize(i32 _width, i32 _height)
{
}

#endif

