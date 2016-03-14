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
using System.Windows.Interop;
using System.Runtime.InteropServices;

namespace XC_FramworkEditor.ResourceViewer
{
    //Use this for the structure
    [StructLayout(LayoutKind.Sequential), Serializable]
    public struct IResource
    {
        public Int32 m_resourecId;

        public EResourceType m_resourceType;

        //char[50]
        [MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 50)]
        public string m_userFriendlyName;

        [MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 50)]
        public string m_resourcePath;

        public bool m_isLoaded;
    }

    //View for ListView
    public class ResourceListViewItem
    {
        public int ResourceId { get; set; }
        public string ResourceType { get; set; }
        public string UserFriendlyName { get; set; }
        public string ResourcePath { get; set; }
        public bool LoadStatus { get; set; }
    }
}
