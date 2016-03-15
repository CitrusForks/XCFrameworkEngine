/* SCE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 02.508.051
* Copyright (C) 2011 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#ifndef __VSOUTPUT_H__
#define __VSOUTPUT_H__

struct VS_OUTPUT
{
	float4 Position		: S_POSITION;
	float3 Color		: TEXCOORD0;
	float2 UV			: TEXCOORD1;
};

#endif