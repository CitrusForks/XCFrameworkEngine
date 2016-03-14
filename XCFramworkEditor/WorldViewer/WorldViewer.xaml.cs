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

namespace XC_FramworkEditor.WorldViewer
{
    /// <summary>
    /// Interaction logic for WorldViewer.xaml
    /// </summary>
    public partial class WorldViewer : Window
    {
        public WorldViewer()
        {
            InitializeComponent();
        }

        protected override void OnActivated(EventArgs e)
        {
            base.OnActivated(e);
            FillWorldViewerListView();
        }

        public void FillWorldViewerListView()
        {
            int noOfActors = PInvokeDeclarations.GetNoOfActors();

            var items = WorldViewerListView.Items;
            items.Clear();
        }
    }
}
