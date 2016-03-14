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

namespace XC_FramworkEditor.BasicTypes
{
    public class Vector3
    {
        float x, y, z;

        public Label m_label { get; set; }
        public TextBox m_tbX { get; set; }
        public TextBox m_tbY { get; set; }
        public TextBox m_tbZ { get; set; }

        public Vector3(string labelName, int rowPosition)
        {
            m_label = new Label();
            m_label.Content = labelName;
            m_label.Height = 20;
            m_label.Foreground = Brushes.Black;
            Grid.SetColumn(m_label, 0);
            Grid.SetRow(m_label, rowPosition);

            m_tbX = new TextBox();
            m_tbX.Width = 40;
            m_tbX.Height = 20;
            Grid.SetColumn(m_tbX, 1);
            Grid.SetRow(m_tbX, rowPosition);

            m_tbY = new TextBox();
            m_tbY.Width = 40;
            m_tbY.Height = 20;
            Grid.SetColumn(m_tbY, 2);
            Grid.SetRow(m_tbY, rowPosition);

            m_tbZ = new TextBox();
            m_tbZ.Width = 40;
            m_tbZ.Height = 20;
            Grid.SetColumn(m_tbZ, 3);
            Grid.SetRow(m_tbZ, rowPosition);
        }

        public void addControlsIntoStackPanel(ref StackPanel panel)
        {
            panel.Children.Add(m_label);
            panel.Children.Add(m_tbX);
            panel.Children.Add(m_tbY);
            panel.Children.Add(m_tbZ);
        }

        public void addControlsIntoGrid(ref Grid grid)
        {
            grid.Children.Add(m_label);
            grid.Children.Add(m_tbX);
            grid.Children.Add(m_tbY);
            grid.Children.Add(m_tbZ);
        }
    }
}
