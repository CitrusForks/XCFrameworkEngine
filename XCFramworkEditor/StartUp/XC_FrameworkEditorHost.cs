/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using System.Runtime.InteropServices;


namespace XC_FramworkEditor.StartUp
{
    public class XC_FrameworkEditorHost : HwndHost 
    {
        //Constants. TODO transfer to


        //Child Window handle
        IntPtr m_hwndHost;

        //Client height and width
        int m_clientWidth;
        int m_clientHeight;

        public XC_FrameworkEditorHost(int width, int height)
        {
            m_hwndHost = IntPtr.Zero;

            m_clientWidth = width;
            m_clientHeight = height;
        }

        protected override System.Runtime.InteropServices.HandleRef BuildWindowCore(System.Runtime.InteropServices.HandleRef hwndParent)
        {
            //Create a child window
            m_hwndHost = PInvokeDeclarations.CreateWindowEx(0, "static", "",
                                                            ConstantDefinitions.WS_CHILD | ConstantDefinitions.WS_VISIBLE,
                                                            0, 0,
                                                            m_clientWidth, m_clientHeight,
                                                            hwndParent.Handle,
                                                            (IntPtr)ConstantDefinitions.HOST_ID,
                                                            IntPtr.Zero,
                                                            0);

            //Set the host window handle to the XC_Framework and launch the game, the game will be rendered in the border area assigned.
            IntPtr hInstance = Marshal.GetHINSTANCE(typeof(App).Module);
            PInvokeDeclarations.Launch_AppFramework_Editor(hInstance, m_hwndHost, m_clientWidth, m_clientHeight);

            return new HandleRef(this, m_hwndHost);
        }

        protected override void DestroyWindowCore(System.Runtime.InteropServices.HandleRef hwnd)
        {

        }

        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        }
    }
}
