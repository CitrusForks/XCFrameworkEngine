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
using System.Windows.Input;

namespace XC_FramworkEditor.Commands.Toolbar
{
    public class ToolbarCommands
    {
        public static readonly RoutedUICommand ResourceViewer = new RoutedUICommand("Resource Viewer", "ResourceViewer", typeof(ToolbarCommands));
        public static readonly RoutedUICommand WorldEditor = new RoutedUICommand("World Editor", "WorldEditor", typeof(ToolbarCommands));

        public static readonly RoutedUICommand Exit = new RoutedUICommand("Exit", "Exit", typeof(ToolbarCommands));
    }
}
