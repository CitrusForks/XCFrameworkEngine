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

using XC_FramworkEditor.BasicTypes;

namespace XC_FramworkEditor.Actors
{
    public class BoxGeometry
    {
        private Vector3 m_position;
        private Vector3 m_rotation;
        private Vector3 m_scaling;
        private CustomList m_rasterTypes;

        private Grid m_grid;

        public BoxGeometry(int startRowIndex, ref Grid grid)
        {
            m_grid = grid;

            m_position = new Vector3("Position :", startRowIndex + 0);
            m_rotation = new Vector3("Rotation :", startRowIndex + 1);
            m_scaling = new Vector3("Scaling :", startRowIndex + 2);
            m_rasterTypes = new CustomList("Raster Type : ", startRowIndex + 3);

            m_position.addControlsIntoGrid(ref m_grid);
            m_rotation.addControlsIntoGrid(ref m_grid);
            m_scaling.addControlsIntoGrid(ref m_grid);
            m_rasterTypes.addControlsIntoGrid(ref m_grid);
        }

        public void OnActivate()
        {
            FillRasterTypes();
        }

        public void FillRasterTypes()
        {
            m_rasterTypes.AddItem("Solid");
            m_rasterTypes.AddItem("Wireframe");
        }
    }
}
