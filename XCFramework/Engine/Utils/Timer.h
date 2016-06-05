/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#if defined(UNUSED)

#include "Engine/System/ISystem.h"
#include "Engine/Graphics/XC_Graphics.h"

class Timer : ISystem
{
public:
    DECLARE_SYSTEMOBJECT_CREATION(Timer)

    Timer()
    {
        m_fps = 0;
        m_perFrameTime = 0.0f;

#if defined(WIN32)
        if (!QueryPerformanceFrequency(&m_frequency))
        {
            XCASSERT(false);
        }
#endif
        Reset();
    }

    void Reset()
    {
        Update();
        m_startTime = m_currentTime;
        m_total = 0.0f;
        m_delta = 1.0f / 60.0f;
        m_fps = 0;
        m_perFrameTime = 0.0f;
    }

    void Update()
    {
#if defined(WIN32)
        if (!QueryPerformanceCounter(&m_currentTime))
        {
            XCASSERT(false);
        }

        m_total = static_cast<float>(
            static_cast<double>(m_currentTime.QuadPart-m_startTime.QuadPart) /
            static_cast<double>(m_frequency.QuadPart)
            );

        if (m_lastTime.QuadPart == m_startTime.QuadPart)
        {
            // If the timer was just reset, report a time delta equivalent to 60Hz frame time.
            m_delta = 1.0f / 60.0f;
        }
        else
        {
            m_delta = static_cast<float>(
                static_cast<double>(m_currentTime.QuadPart-m_lastTime.QuadPart) /
                static_cast<double>(m_frequency.QuadPart)
                );
        }
#endif

        m_lastTime = m_currentTime;

        //Calculate fps
        m_perFrameTime += m_delta;
        m_fps++;

        if (m_perFrameTime >= 1.0f)
        {
#if defined(WIN32)
            char fpsStr[256];
            sprintf_s(fpsStr, "\t FPS : %d", m_fps);

            XC_Graphics& graphics = SystemLocator::GetInstance()->RequestSystem<XC_Graphics>("GraphicsSystem");
            graphics.SetWindowTitle(graphics.GetDefaultWindowTitle() + fpsStr);

            m_fps = 0;
            m_perFrameTime = 0.0f;
#endif
        }
    }

    float GetTotalTimeSpent() { return m_total; }

    float getDeltaTime() { return m_delta; }

private:

    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_currentTime;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_lastTime;

    float         m_total;
    float         m_delta;
    float         m_perFrameTime;
    int           m_fps;
};

#endif