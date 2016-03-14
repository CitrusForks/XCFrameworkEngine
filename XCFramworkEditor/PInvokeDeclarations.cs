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

namespace XC_FramworkEditor
{
    public class PInvokeDeclarations
    {
        //CreateWindowEx function declaration
        [DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Unicode)]
        internal static extern IntPtr CreateWindowEx(int dwExStyle,
                                                      string lpszClassName,
                                                      string lpszWindowName,
                                                      int style,
                                                      int x, int y,
                                                      int width, int height,
                                                      IntPtr hwndParent,
                                                      IntPtr hMenu,
                                                      IntPtr hInst,
                                                      [MarshalAs(UnmanagedType.AsAny)] object pvParam);


        //Main Entry point for Editor host
        [DllImport("XC_Framework.dll", EntryPoint = "Launch_AppFramework_Editor", CharSet = CharSet.Ansi)]
        internal static extern void Launch_AppFramework_Editor(IntPtr hInstance, IntPtr hwnd, int width, int height);


        //ResourceViewer
        [DllImport("XC_Framework.dll", EntryPoint = "GetNoOfResources", CharSet = CharSet.Ansi)]
        internal static extern int GetNoOfResources();

        [DllImport("XC_Framework.dll", EntryPoint = "GetResourceList", CharSet = CharSet.Ansi)]
        internal static extern void GetResourceList(IntPtr ptrToList);

        [DllImport("XC_Framework.dll", EntryPoint = "GetResourceAt", CharSet = CharSet.Ansi)]
        internal static extern IntPtr GetResourceAt(int index);

        //WorldViewer
        [DllImport("XC_Framework.dll", EntryPoint = "GetNoOfActors", CharSet = CharSet.Ansi)]
        internal static extern int GetNoOfActors();

        //WorldEditor
        [DllImport("XC_Framework.dll", EntryPoint = "GetNoOfActorTypes", CharSet = CharSet.Ansi)]
        internal static extern int GetNoOfActorTypes();

        [DllImport("XC_Framework.dll", EntryPoint = "GetAllActorTypes", CharSet = CharSet.Ansi)]
        internal static extern void GetAllActorTypes(IntPtr actorList);
    }
}
