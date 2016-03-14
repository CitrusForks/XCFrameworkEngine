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
using System.Windows.Navigation;
using System.Windows.Shapes;

using XC_FramworkEditor.StartUp;
using System.Windows.Interop;
using XC_FramworkEditor.Commands.Toolbar;
using XC_FramworkEditor.WorldEditor;


namespace XC_FramworkEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Application m_App;

        XC_FrameworkEditorHost m_EditorHost;

        //Windows
        ResourceViewerWindow m_resourceViewerWindow;
        WorldEditor.WorldEditor m_worldEditorWindow;

        public MainWindow()
        {
            InitializeComponent();

            //Initialize all Windows
            m_resourceViewerWindow = new ResourceViewerWindow();
            m_worldEditorWindow = new WorldEditor.WorldEditor();

        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            //Initialize hosted XC_Framework
            m_App = System.Windows.Application.Current;
            m_EditorHost = new XC_FrameworkEditorHost((int) EditorHostBorder.Width, (int) EditorHostBorder.Height);
            EditorHostBorder.Child = m_EditorHost;
        }

        private void OnResourceViewerExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void OnResourceViewerExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Console.WriteLine("[OnResourceViewer Button CLicked and executed]");
            m_resourceViewerWindow.Show();
        }

        private void OnWorldEditorExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void OnWorldEditorExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Console.WriteLine("[OnResourceViewer Button CLicked and executed]");
            m_worldEditorWindow.Show();
        }
    }
}
