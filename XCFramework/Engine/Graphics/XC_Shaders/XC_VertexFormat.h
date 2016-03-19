/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once

#include "Engine/Graphics/XC_Shaders/XC_VertexShaderLayout.h"

#if defined(XC_ORBIS)

#include "XC_VertexFormatOrbis.h"

#else

enum EVertexFormat
{
    VertexFormat_Position,
    VertexFormat_PositionColor,
    VertexFormat_PositionNormal,
    VertexFormat_PositionNormalTexture,
    VertexFormat_PositionNormalTextureBlendIndexBlendWeight,
    VertexFormat_PositionColorInstanceIndex,

    VertexFormat_Invalid
};

struct SemanticVariable
{
    SemanticVariable(EVertexFormat format, std::vector<std::string> semanticName)
        : m_formatType(format),
          m_semanticPattern(semanticName)
    {}

    EVertexFormat              m_formatType;
    std::vector<std::string>   m_semanticPattern;
};

static const SemanticVariable gs_sematicVariables[] = {
    SemanticVariable(VertexFormat_Position,                                   { "POSITION" }),
    SemanticVariable(VertexFormat_PositionColor,                              { "POSITION", "COLOR" }), 
    SemanticVariable(VertexFormat_PositionNormal,                             { "POSITION", "NORMAL" }),
    SemanticVariable(VertexFormat_PositionNormalTexture,                      { "POSITION", "NORMAL", "TEXCOORD" }),
    SemanticVariable(VertexFormat_PositionNormalTextureBlendIndexBlendWeight, { "POSITION", "NORMAL", "TEXCOORD", "BLENDINDICES", "BLENDWEIGHT" }),
    SemanticVariable(VertexFormat_PositionColorInstanceIndex,                 { "POSITION", "COLOR", "SV_InstanceID" })
};

static EVertexFormat getVertexFormatFromSematicNames(std::vector<std::string>& semanticNames)
{
    for(auto semantic : gs_sematicVariables)
    {
        if (semantic.m_semanticPattern.size() == semanticNames.size() && std::equal(semantic.m_semanticPattern.begin(), semantic.m_semanticPattern.end(), semanticNames.begin()))
        {
            return semantic.m_formatType;
        }
    }
    XCASSERT(false);
    return VertexFormat_Invalid;
}

static D3D12_INPUT_LAYOUT_DESC getInputLayoutFromSemantics(std::vector<std::string>& semanticNames)
{
    D3D12_INPUT_LAYOUT_DESC layout = {};

    EVertexFormat format = getVertexFormatFromSematicNames(semanticNames);

    switch (format)
    {
    case VertexFormat_Position:
        layout.pInputElementDescs = VertexPosInputLayoutDesc;
        layout.NumElements = ARRAYSIZE(VertexPosInputLayoutDesc);
        break;
    case VertexFormat_PositionColor:
        layout.pInputElementDescs = VertexPosColorInputLayoutDesc;
        layout.NumElements = ARRAYSIZE(VertexPosColorInputLayoutDesc);
        break;

    case VertexFormat_PositionNormal:
        layout.pInputElementDescs = VertexPosNormInputLayoutDesc;
        layout.NumElements = ARRAYSIZE(VertexPosNormInputLayoutDesc);
        break;

    case VertexFormat_PositionNormalTexture:
        layout.pInputElementDescs = VertexPosNormTexInputLayoutDesc;
        layout.NumElements = ARRAYSIZE(VertexPosNormTexInputLayoutDesc);
        break;

    case VertexFormat_PositionNormalTextureBlendIndexBlendWeight:
        layout.pInputElementDescs = VertexPosNormTexBIndexBWeightInputLayoutDesc;
        layout.NumElements = ARRAYSIZE(VertexPosNormTexBIndexBWeightInputLayoutDesc);
        break;

    case VertexFormat_PositionColorInstanceIndex:
        layout.pInputElementDescs = VertexColorInstIdxInputLayoutDesc;
        layout.NumElements = ARRAYSIZE(VertexColorInstIdxInputLayoutDesc);
        break;

    case VertexFormat_Invalid:
    default:
        XCASSERT(false);
        break;
    }

    return layout;
}
 
typedef struct VertexPosColorTex
{
    float x, y, z;	// Position
    float r, g, b;	// Color
    float u, v;		// UVs

    VertexPosColorTex(float _x, float _y, float _z, float _r, float _g, float _b, float _u, float _v)
        : x(_x), y(_y), z(_z), r(_r), g(_g), b(_b), u(_u), v(_v)
    {
    }
} VertexPosColorTex;

struct VertexPosColor
{
    XCVec3Unaligned  Pos;
    XCVec4Unaligned  Color;

    VertexPosColor() :
        Pos(0, 0, 0),
        Color(0, 0, 0, 0)
    {
    }

    VertexPosColor(XCVec3Unaligned _pos, XCVec4Unaligned _color) :
        Pos(_pos),
        Color(_color)
    {
    }
};

struct VertexPosNorm
{
    XCVec3Unaligned Pos;
    XCVec3Unaligned Norm;

    VertexPosNorm() :
        Pos(0, 0, 0),
        Norm(0, 0, 0)
    {
    }

    VertexPosNorm(XCVec3Unaligned pos, XCVec3Unaligned norm) :
        Pos(pos),
        Norm(norm)
    {
    }
};

struct VertexPosNormTex
{
    XCVec3Unaligned Pos;
    XCVec3Unaligned Norm;
    XCVec2Unaligned Tex;

    VertexPosNormTex() :
        Pos(0, 0, 0),
        Norm(0, 0, 0),
        Tex(0, 0)
    {
    }

    VertexPosNormTex(XCVec3Unaligned _pos, XCVec3Unaligned _normal, XCVec2Unaligned _tex) :
        Pos(_pos),
        Norm(_normal),
        Tex(_tex)
    {
    }
};

struct VertexPosNormTexBIndexBWeight
{
    XCVec3Unaligned Pos;
    XCVec3Unaligned Norm;
    XCVec2Unaligned Tex;
    XCVec4Unaligned BlendIndices;
    XCVec4Unaligned BlendWeights;

    VertexPosNormTexBIndexBWeight() :
        Pos(0, 0, 0),
        Norm(0, 0, 0),
        Tex(0, 0),
        BlendIndices(0, 0, 0, 0),
        BlendWeights(0, 0, 0, 0)
    {
    }

    VertexPosNormTexBIndexBWeight(XCVec3Unaligned _pos, XCVec3Unaligned _normal, XCVec2Unaligned _tex, XCVec4Unaligned bIndex, XCVec4Unaligned bWeight) :
        Pos(_pos),
        Norm(_normal),
        Tex(_tex),
        BlendIndices(bIndex),
        BlendWeights(bWeight)
    {
    }
};

struct VertexPosColorInstanceIndex
{
    /*struct InstanceBuffer
    {
        XCVec4Unaligned SamplePad;
    };*/

    XCVec4Unaligned Pos;
    XCVec4Unaligned Color;

    VertexPosColorInstanceIndex() :
        Pos(0, 0, 0, 0),
        Color(0, 0, 0, 0)
    {
    }

    VertexPosColorInstanceIndex(XCVec4Unaligned _pos, XCVec4Unaligned color) :
        Pos(_pos),
        Color(color)
    {
    }
};

#endif