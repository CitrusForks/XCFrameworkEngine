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
using XC_FramworkEditor.BasicTypes;
using XC_FramworkEditor.Actors;

namespace XC_FramworkEditor.WorldEditor
{
    /// <summary>
    /// Interaction logic for WorldEditor.xaml
    /// </summary>
    public partial class WorldEditor : Window
    {
        private static int m_noOfRowCols = 10;
        private string[] m_actorTypeList;
        private bool m_initialized;
        private bool m_showingResourceSelector;
        private ResourceViewerWindow m_resourceSelectorWindow;
        private ResourceListViewItem m_resourceItem;

        private Grid m_grid;

        private CustomList m_actorList;
        private BoxGeometry m_boxGeometryActor;

        [StructLayout(LayoutKind.Sequential), Serializable]
        public struct GameActorInfo
        {
            [MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 50)]
            public string actorName;
        }

        public WorldEditor()
        {
            InitializeComponent();
            m_initialized = false; 
            m_showingResourceSelector = false;
            m_resourceSelectorWindow = new ResourceViewerWindow();
            
            InitGrid();

            m_actorList = new CustomList("Actor :", 0);
            m_actorList.addControlsIntoGrid(ref m_grid);

            m_boxGeometryActor = new BoxGeometry(1, ref m_grid);

            this.Content = m_grid;
        }

        private void InitGrid()
        {
            m_grid = new Grid();

            m_grid.HorizontalAlignment = HorizontalAlignment.Left;
            m_grid.VerticalAlignment = VerticalAlignment.Top;

            for (int index = 0; index < m_noOfRowCols; index++)
            {
                ColumnDefinition col = new ColumnDefinition();
                col.Width = GridLength.Auto;

                m_grid.ColumnDefinitions.Add(col);

                RowDefinition row = new RowDefinition();
                row.Height = GridLength.Auto;

                m_grid.RowDefinitions.Add(row);
            }
        }

        protected override void OnActivated(EventArgs e)
        {
            base.OnActivated(e);

            if(!m_initialized)
            {
                //Get all the actor types
                FillActorTypes();
                m_boxGeometryActor.OnActivate();

                m_initialized = true;
            }
        }

        public void FillActorTypes()
        {
            int noOfActors = PInvokeDeclarations.GetNoOfActorTypes();

            //Clear currently all the resources
            var items = ActorTypesComboBox.Items;
            items.Clear();

           
            m_actorTypeList = new string[noOfActors];

            GameActorInfo actorInfo = new GameActorInfo();
            int sizeofRP = Marshal.SizeOf(typeof(GameActorInfo));
            IntPtr pRP = Marshal.AllocHGlobal(sizeofRP * noOfActors);
            Marshal.StructureToPtr(actorInfo, pRP, false);
            PInvokeDeclarations.GetAllActorTypes(pRP);

            for (int index = 0; index < noOfActors; index++)
            {
                actorInfo = (GameActorInfo) Marshal.PtrToStructure(pRP + (index * Marshal.SizeOf(typeof(GameActorInfo))), typeof(GameActorInfo));
                m_actorTypeList[index] = actorInfo.actorName;
                m_actorList.AddItem(actorInfo.actorName);
            }

            Marshal.FreeHGlobal(pRP);
        }



        public void AddActor()
        {
        }

        private void OnResourceViewerExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;

            if(m_showingResourceSelector)
            {
                m_resourceItem = m_resourceSelectorWindow.getCurrentSelectedResource();
                if (m_resourceItem != null)
                {
                    ResourceSelectorTextBox.Text = m_resourceItem.UserFriendlyName;
                }
            }
        }

        private void OnResourceViewerExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            m_showingResourceSelector = true;
            Console.WriteLine("[OnResourceViewer Button CLicked and executed]");
            m_resourceSelectorWindow.Show();
        }
    }
}
