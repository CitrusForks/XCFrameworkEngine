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
    public class CustomList
    {
        private Label m_comboLabel;
        private ComboBox m_comboBoxList;
        private int m_currentSelected;

        public CustomList(string labelName, int rowIndex)
        {
            m_currentSelected = -1;

            m_comboLabel = new Label { Content = labelName, Foreground = Brushes.Black };

            Grid.SetRow(m_comboLabel, rowIndex);
            Grid.SetColumn(m_comboLabel, 0);

            m_comboBoxList = new ComboBox { Width = 120 };
            m_comboBoxList.Items.Clear();
            Grid.SetRow(m_comboBoxList, rowIndex);
            Grid.SetColumn(m_comboBoxList, 1);
            Grid.SetColumnSpan(m_comboBoxList, 3);
        }

        public void AddItem(string item)
        {
            m_comboBoxList.Items.Add(item);
        }

        public void addControlsIntoGrid(ref Grid grid)
        {
            grid.Children.Add(m_comboLabel);
            grid.Children.Add(m_comboBoxList);
        }
    }
}
