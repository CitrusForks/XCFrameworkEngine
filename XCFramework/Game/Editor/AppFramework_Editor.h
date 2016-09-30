#pragma once

#include "Engine/ApplicationFramework/IAppFramework.h"

class AppFramework_Editor : public IAppFramework
{
public:
        AppFramework_Editor(HINSTANCE hInstance, std::string winCaption, HWND hwnd, int width, int height);

        virtual ~AppFramework_Editor();

        int                     Run();

        //Framework Methods
        virtual void            Init();

        virtual void            OnResize() = 0;
        virtual void            UpdateScene(float dt) = 0;
        virtual void            DrawScene() = 0;

        virtual void            Destroy();

        virtual HWND            CreateD3DChildWindow();

        void                    EnableFullScreenMode(bool enable);
        bool                    IsDeviceLost();

protected:
        //Override these methods please if you need to change working

        virtual void            InitMainWindow();
        virtual void            CalculateFramesPerSecond(float dt){}	//Call from update method
};