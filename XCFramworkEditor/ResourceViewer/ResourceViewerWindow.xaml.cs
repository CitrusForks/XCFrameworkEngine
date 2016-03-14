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
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

using System.Windows.Interop;
using System.Runtime.InteropServices;

using XC_FramworkEditor.ResourceViewer;

namespace XC_FramworkEditor
{
    /// <summary>
    /// Interaction logic for ResourceViewerWindow.xaml
    /// </summary>
    /// 
    

    public partial class ResourceViewerWindow : Window
    {

        public ResourceViewerWindow()
        {
            InitializeComponent();
        }

         protected override void OnActivated(EventArgs e)
         {
             base.OnActivated(e);

             FillResourceViewerListView();
         }

         public void FillResourceViewerListView()
         {
             //Request the Resource Manager to get the list of resources.
             int noOfResources = PInvokeDeclarations.GetNoOfResources();

             //Clear currently all the resources
             var items = ResourceViewerListView.Items;
             items.Clear();

             IResource payload = new IResource();

             int sizeofRP = Marshal.SizeOf(typeof(IResource));
             IntPtr pRP = Marshal.AllocHGlobal(sizeofRP * noOfResources);
             Marshal.StructureToPtr(payload, pRP, false);
             PInvokeDeclarations.GetResourceList(pRP);

             for (int index = 0; index < noOfResources; index++)
             {
                 payload = (IResource) Marshal.PtrToStructure(pRP + (index * Marshal.SizeOf(typeof(IResource))), typeof(IResource));

                 items.Add(new ResourceListViewItem
                 {
                     ResourceId = payload.m_resourecId,
                     ResourceType = payload.m_resourceType.ToString(),
                     UserFriendlyName = payload.m_userFriendlyName,
                     ResourcePath = payload.m_resourcePath,
                     LoadStatus = payload.m_isLoaded
                 });
             }

             Marshal.FreeHGlobal(pRP);
         }

        public ResourceListViewItem getCurrentSelectedResource()
         {
             if (ResourceViewerListView.SelectedIndex == -1)
                 return null;

            return ResourceViewerListView.SelectedItems[0] as ResourceListViewItem;
         }
    }
}
