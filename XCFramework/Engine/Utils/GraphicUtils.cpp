/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "stdafx.h"
#include "Engine/Utils/GraphicUtils.h"

const WCHAR* charToWideChar(const char* str)
{
    const size_t cSize = strlen(str) + 1;

    std::wstring wc(cSize, L'#');
    mbstowcs(&wc[0], str, cSize);

    return wc.c_str();
}

float XMVec3CosineDot(XCVecIntrinsic4& vec1, XCVecIntrinsic4& vec2)
{
    XCVec3 v1, v2;
    XMStoreFloat3(&v1, vec1);
    XMStoreFloat3(&v2, vec2);

#if defined(WIN32)
    return acosf(XMVectorGetY(XMVector3Dot(vec1, vec2))/ (sqrt(((v1.x*v1.x) + (v1.y*v1.y) + (v1.z*v1.z))*((v2.x*v2.x) + (v2.y*v2.y) + (v2.z*v2.z)))));
#elif defined(XC_ORBIS)
    return acosf(XMVectorGetY(XMVector3Dot(vec1, vec2)) / (sqrt(((v1.getX()*v1.getX()) + (v1.getY()*v1.getY()) + (v1.getZ()*v1.getZ()))*((v2.getX()*v2.getX()) + (v2.getY()*v2.getY()) + (v2.getZ()*v2.getZ())))));
#endif
}

XCVecIntrinsic4 CreatePlaneFromPoints(XCVecIntrinsic4 p1, XCVecIntrinsic4 p2, XCVecIntrinsic4 p3)
{
	XCVecIntrinsic4 v1 = XMVector3Normalize(p1);
	XCVecIntrinsic4 v2 = XMVector3Normalize(p2);
	XCVecIntrinsic4 v3 = XMVector3Normalize(p3);

	XCVecIntrinsic4 axis1 = v1 - v2; // from 1 to 0
	XCVecIntrinsic4 axis2 = v3 - v2; // from 1 to 2

	return XMVector3Normalize(XMVector3Cross(axis1, axis2));
}

XCVecIntrinsic4 GetNormalFromPoints(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2, XCVecIntrinsic4 v3)
{
	//Find the 2 axis for v1 vertex
	XCVecIntrinsic4 axis1 = v2 - v1;
	XCVecIntrinsic4 axis2 = v3 - v1;

	//Normalize the axis
	XMVector3Normalize(axis1);
	XMVector3Normalize(axis2);

	//Now find the cross product of both the axis
	return XMVector3Normalize(XMVector3Cross(axis1, axis2));
}

bool IsVectorEqual(XCVecIntrinsic4 v1, XCVecIntrinsic4 v2)
{
	if (XMVectorGetX(v1) > XMVectorGetX(v2) || XMVectorGetX(v1) < XMVectorGetX(v2)) return false;
	if (XMVectorGetY(v1) > XMVectorGetY(v2) || XMVectorGetY(v1) < XMVectorGetY(v2)) return false;
	if (XMVectorGetZ(v1) > XMVectorGetZ(v2) || XMVectorGetZ(v1) < XMVectorGetZ(v2)) return false;
	
	return true;
}

unsigned int getFilelength(FILE* fp)
{
    unsigned originalPosition = ftell(fp);
    fseek(fp, 0, SEEK_END);
    unsigned int endPosition = ftell(fp);

    fseek(fp, originalPosition, SEEK_SET);
    return endPosition;
}

HRESULT ReadRawDataFromFile(const char* filename, UINT8** data, unsigned int& size)
{
    FILE* fp = fopen(filename, "rb");

    XCASSERT(fp);

    if (fp != nullptr)
    {
        //Get length of data
        unsigned int length = getFilelength(fp);

        *data = new UINT8[length];
        int res = fread(*data, sizeof(byte), length, fp);
        XCASSERT(res > 0);

        size = res;
        fclose(fp);
    }

    return S_OK;
}