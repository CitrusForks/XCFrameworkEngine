
cbuffer cbSettings
{
    //Compute shader can access values in constant buffer
};

//Data sources and o/p

Texture2D gInputA;
Texture2D gInputB;

RWTexture2D<float4> gOutput;

//Arrangement of thread groups
[numthreads(16, 16, 1)]
void CS(int3 dispatchThreadID : SV_DispatchThreadID)    //ThreadID
{
    //Sum the xyth texels and store the result in the xyth texel of gOutput
    gOutput[dispatchThreadID.xy] = gInputA[dispatchThreadID.xy] + gInputB[dispatchThreadID.xy];
}


technique11 AddTextures
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}