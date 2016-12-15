/* XCFrameworkEngine
* Copyright (C) Abhishek Porwal, 2016
* Any queries? Contact author <https://github.com/abhishekp314>
* This program is complaint with GNU General Public License, version 3.
* For complete license, read License.txt in source root directory. */

#pragma once

struct Vertex
{
    f32 x;
    f32 y;
    f32 z;
};

struct MapCoord
{
    f32 u;
    f32 v;
};

struct Face
{
    Face(u16 x, u16 y, u16 z, u16 w = 0)
        : a(x)
        , b(y)
        , c(z)
        , flags(w)
    {}

    u16 a;
    u16 b;
    u16 c;

    u16 flags;
};

struct MeshInfo
{
    static const MeshInfo EmptyInfo;

    std::vector<Vertex>              m_vertices;
    std::vector<u32>                 m_indices;
};