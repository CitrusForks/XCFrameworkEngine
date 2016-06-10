/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"

#include "XC_ShaderHandle.h"
#include "XC_VertexFormat.h"
#include "XC_RasterizerTypes.h"

#include "Graphics/XC_Textures/Texture2D.h"
#include "Graphics/XC_Textures/CubeTexture3D.h"

#include "Assets/Packages/PackageConsts.h"

//#define LOG_SHADER_SLOTS 1

XCShaderHandle::XCShaderHandle(ID3DDevice& device) 
    : IShader(device)
{
#if defined(XCGRAPHICS_DX12)
    m_pso = XCNEW(PSO_Dx12);
#endif

    m_vertexFormat = VertexFormat_Invalid;
}

XCShaderHandle::~XCShaderHandle()
{
    ReleaseCOM(m_vsShaderReflection);
    ReleaseCOM(m_psShaderReflection);

    if (m_pVS)
    {
        delete(m_pVS);
    }

    if (m_pPS)
    {
        delete(m_pPS);
    }

#if defined(XCGRAPHICS_DX12)
    if (m_pso)
    {
        XCDELETE(m_pso);
    }
#endif
}

void XCShaderHandle::Load(const void* fbBuffer)
{
    IShader::Load(fbBuffer);

    const FBShader* fbShader = (FBShader*)fbBuffer;
    m_enableDepth = fbShader->EnableDepth();

    LoadShader();
}

void XCShaderHandle::LoadShader()
{
    //Compile and reflect the shader
#if defined(USE_D3D_COMPILER)
    //Load the shader
    UINT compilerFlags = 0;

#if defined(_DEBUG)
    compilerFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ValidateResult(D3DCompileFromFile(m_vertexShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compilerFlags, 0, &m_pVS, &errors));
    ReleaseCOM(errors);
    ValidateResult(D3DCompileFromFile(m_pixelShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compilerFlags, 0, &m_pPS, &errors));
    ReleaseCOM(errors);
#else
    ValidateResult(ReadRawDataFromFile(m_vertexShaderPath.c_str(), &m_pVS, m_vsSize));
    ValidateResult(ReadRawDataFromFile(m_pixelShaderPath.c_str(), &m_pPS, m_psSize));
#endif

    XCASSERT(m_pVS || m_pPS);

    //Fetch information on vs
    ValidateResult(D3DReflect(m_pVS, m_vsSize, IID_ID3D11ShaderReflection, (void**)& m_vsShaderReflection));
    ValidateResult(D3DReflect(m_pPS, m_psSize, IID_ID3D11ShaderReflection, (void**)& m_psShaderReflection));

    ReadShaderDescription();
    GenerateRootSignature();
    GeneratePSO();
    
    Logger("Shader %s Created", m_vertexShaderPath.c_str());
}

void XCShaderHandle::ReadShaderDescription()
{
    //Read the shader descriptions
    D3D_SHADER_DESC vsShaderDesc = {};
    m_vsShaderReflection->GetDesc(&vsShaderDesc);

    D3D_SHADER_DESC psShaderDesc = {};
    m_psShaderReflection->GetDesc(&psShaderDesc);

    for (unsigned int cbIndex = 0; cbIndex < vsShaderDesc.ConstantBuffers; ++cbIndex)
    {
        D3D_SHADER_BUFFER_DESC desc = {};
        m_vsShaderReflection->GetConstantBufferByIndex(cbIndex)->GetDesc(&desc);

        if (std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(), [desc](ShaderSlot it) -> bool { return strcmp(it.GetBufferName().c_str(), desc.Name) == 0; }) == m_shaderSlots.end())
        {
            //Push the cb desc to our map with its name
            m_shaderSlots.push_back(ShaderSlot(desc));
        }

#if defined(LOG_SHADER_SLOTS)
        Logger("[Constant Buffer] Name : %s, Size : %d, Type : %d, Flags : %d, Member Variables : %d", desc.Name, desc.Size, desc.Type, desc.uFlags, desc.Variables);
#endif
    }

    for (unsigned int cbIndex = 0; cbIndex < psShaderDesc.ConstantBuffers; ++cbIndex)
    {
        D3D_SHADER_BUFFER_DESC desc = {};
        m_psShaderReflection->GetConstantBufferByIndex(cbIndex)->GetDesc(&desc);

        if (std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(), [&desc](ShaderSlot it) -> bool { return strcmp(it.GetBufferName().c_str(), desc.Name) == 0; }) == m_shaderSlots.end())
        {
            //Push the cb desc to our map with its name
            m_shaderSlots.push_back(ShaderSlot(desc));
        }

#if defined(LOG_SHADER_SLOTS)
        Logger("[Constant Buffer] Name : %s, Size : %d, Type : %d, Flags : %d, Member Variables : %d", desc.Name, desc.Size, desc.Type, desc.uFlags, desc.Variables);
#endif
    }

    //Read the resource slots from vs.
    for (unsigned int resBoundIndex = 0; resBoundIndex < vsShaderDesc.BoundResources; ++resBoundIndex)
    {
        D3D_SHADER_INPUT_BIND_DESC inputDesc = {};
        m_vsShaderReflection->GetResourceBindingDesc(resBoundIndex, &inputDesc);

        if (std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(), [&inputDesc](ShaderSlot it) -> bool { return strcmp(it.GetBufferName().c_str(), inputDesc.Name) == 0; }) == m_shaderSlots.end())
        {
#if defined(LOG_SHADER_SLOTS)
            Logger("[Resource Bound] Name : %s, Type : %d, Dimensions : %d", inputDesc.Name, inputDesc.Type, inputDesc.Dimension);
#endif
            m_shaderSlots.push_back(ShaderSlot(inputDesc));
        }
    }


    //Read the resource slots from ps.
    for (unsigned int resBoundIndex = 0; resBoundIndex < psShaderDesc.BoundResources; ++resBoundIndex)
    {
        D3D_SHADER_INPUT_BIND_DESC inputDesc = {};
        m_psShaderReflection->GetResourceBindingDesc(resBoundIndex, &inputDesc);

        if (std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(), [&inputDesc](ShaderSlot it) -> bool { return strcmp(it.GetBufferName().c_str(), inputDesc.Name) == 0; }) == m_shaderSlots.end())
        {
#if defined(LOG_SHADER_SLOTS)
            Logger("[Resource Bound] Name : %s, Type : %d, Dimensions : %d", inputDesc.Name, inputDesc.Type, inputDesc.Dimension);
#endif
            m_shaderSlots.push_back(ShaderSlot(inputDesc));
        }
    }

    //Sort the slots according to a priority
    SortSlots();

    for (unsigned int slotIndex = 0; slotIndex < m_shaderSlots.size(); ++slotIndex)
    {
        Logger("Slot [ %d ]  : Variable : %s", slotIndex, m_shaderSlots[slotIndex].GetBufferName().c_str());
    }

    //-----------------------------------------------------------------------------------------------------------
    //Generate the input layout
    std::vector<std::string> semanticNames;

    //Input parameters are suppose to be of vertex shader.
    for (unsigned int ipParamIndex = 0; ipParamIndex < vsShaderDesc.InputParameters; ++ipParamIndex)
    {
        D3D_SIGNATURE_PARAMETER_DESC desc = {};
        m_vsShaderReflection->GetInputParameterDesc(ipParamIndex, &desc);
        m_shaderInputParams.push_back(desc);
        semanticNames.push_back(desc.SemanticName);
    }

    m_vertexFormat    = GetVertexFormatFromSematicNames(semanticNames);
    m_inputLayoutDesc = GetInputLayoutFromVertexFormat(m_vertexFormat);
}

unsigned int XCShaderHandle::GetSizeOfConstantBuffer(std::string& cbName)
{
    unsigned int cbSize = 0;
    auto shaderSlot = std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(), [&cbName](ShaderSlot it) -> bool { return strcmp(it.GetBufferName().c_str(), cbName.c_str()) == 0; });
        
    if (shaderSlot != m_shaderSlots.end())
    {
        cbSize = (*shaderSlot).m_bufferDesc.constantBufferDesc.Size;
    }

    XCASSERT(cbSize != 0);
    return cbSize;
}

void XCShaderHandle::GenerateRootSignature()
{
#if defined(XCGRAPHICS_DX12)
    //Create no of desc ranges and root params
    std::vector<CD3DX12_DESCRIPTOR_RANGE> descRanges(m_shaderSlots.size(), CD3DX12_DESCRIPTOR_RANGE());
    std::vector<CD3DX12_ROOT_PARAMETER>   rootParams(m_shaderSlots.size(), CD3DX12_ROOT_PARAMETER());

    unsigned int cbCt = 0;
    unsigned int texCt = 0;
    unsigned int samCt = 0;

    for (unsigned int slotIndex = 0; slotIndex < m_shaderSlots.size(); ++slotIndex)
    {
        //What type?
        BufferType type = m_shaderSlots[slotIndex].m_bufferType;

        switch (type)
        {
        case BUFFERTYPE_CBV:
        {
            descRanges[slotIndex].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, cbCt++);
            rootParams[slotIndex].InitAsDescriptorTable(1, &descRanges[slotIndex], D3D12_SHADER_VISIBILITY_ALL);
        }
        break;

        case BUFFERTYPE_SRV:
        {
            switch (m_shaderSlots[slotIndex].m_bufferDesc.resourceBufferDesc.Type)
            {
            case D3D_SIT_TEXTURE:
            {
                descRanges[slotIndex].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, texCt++);
                rootParams[slotIndex].InitAsDescriptorTable(1, &descRanges[slotIndex], D3D12_SHADER_VISIBILITY_ALL);
            }
            break;

            case D3D_SIT_SAMPLER:
            {
                descRanges[slotIndex].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, samCt++);
                rootParams[slotIndex].InitAsDescriptorTable(1, &descRanges[slotIndex], D3D12_SHADER_VISIBILITY_ALL);
            }
            break;

            default:
                XCASSERT(false);
                break;
            }
        }
        break;

        default:
            XCASSERT(false);
            break;
        }
    }

    //PSO
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.NumParameters = rootParams.size();
    rootSignatureDesc.pParameters = &rootParams[0];
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
        | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
        | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    ID3DBlob* signature;
    ID3DBlob* errors;

    ValidateResult(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errors));
    
    m_pso->CreateRootSignature(m_device, signature->GetBufferPointer(), signature->GetBufferSize(), PSOType_RASTER_FILL_SOLID);
    m_pso->CreateRootSignature(m_device, signature->GetBufferPointer(), signature->GetBufferSize(), PSOType_RASTER_FILL_WIREFRAME);

    ReleaseCOM(signature);
    ReleaseCOM(errors);
#endif
}

void XCShaderHandle::GeneratePSO()
{
#if defined(XCGRAPHICS_DX12)
    //Generate PSO
    PSO_Dx12::GenerateDefaultPSO(m_pso, PSOType_RASTER_FILL_SOLID);
    PSO_Dx12::GenerateDefaultPSO(m_pso, PSOType_RASTER_FILL_WIREFRAME);

    m_pso->m_psos[PSOType_RASTER_FILL_SOLID].m_psoDesc.InputLayout     = m_inputLayoutDesc;
    m_pso->m_psos[PSOType_RASTER_FILL_WIREFRAME].m_psoDesc.InputLayout = m_inputLayoutDesc;

#if defined(USE_D3D_COMPILER)
    m_pso->m_psos[PSOType_RASTER_FILL_SOLID].m_psoDesc.VS = { reinterpret_cast<UINT8*>(m_pVS->GetBufferPointer()), m_pVS->GetBufferSize() };
    m_pso->m_psos[PSOType_RASTER_FILL_SOLID].m_psoDesc.PS = { reinterpret_cast<UINT8*>(m_pPS->GetBufferPointer()), m_pPS->GetBufferSize() };
#else
    m_pso->m_psos[PSOType_RASTER_FILL_SOLID].m_psoDesc.VS = { m_pVS, m_vsSize };
    m_pso->m_psos[PSOType_RASTER_FILL_SOLID].m_psoDesc.PS = { m_pPS, m_psSize };
#endif

#if defined(USE_D3D_COMPILER)
    m_pso->m_psos[PSOType_RASTER_FILL_WIREFRAME].m_psoDesc.VS = { reinterpret_cast<UINT8*>(m_pVS->GetBufferPointer()), m_pVS->GetBufferSize() };
    m_pso->m_psos[PSOType_RASTER_FILL_WIREFRAME].m_psoDesc.PS = { reinterpret_cast<UINT8*>(m_pPS->GetBufferPointer()), m_pPS->GetBufferSize() };
#else
    m_pso->m_psos[PSOType_RASTER_FILL_WIREFRAME].m_psoDesc.VS = { m_pVS, m_vsSize };
    m_pso->m_psos[PSOType_RASTER_FILL_WIREFRAME].m_psoDesc.PS = { m_pPS, m_psSize };
#endif

    //Check the depth enable.
    m_pso->m_psos[PSOType_RASTER_FILL_SOLID].m_psoDesc.DepthStencilState.DepthEnable     = m_enableDepth;
    m_pso->m_psos[PSOType_RASTER_FILL_WIREFRAME].m_psoDesc.DepthStencilState.DepthEnable = m_enableDepth;

    m_pso->CreateGraphicPSO(m_device, PSOType_RASTER_FILL_SOLID);
    m_pso->CreateGraphicPSO(m_device, PSOType_RASTER_FILL_WIREFRAME);

#elif defined(XCGRAPHICS_DX11)
    ValidateResult(m_device.CreateInputLayout(m_inputLayoutDesc.pInputElementDescs, m_inputLayoutDesc.NumElements, m_pVS, m_vsSize, &m_inputLayout));
#endif
}

void XCShaderHandle::SortSlots()
{
    for (unsigned int slotIndex = 0; slotIndex < m_shaderSlots.size() - 1; ++slotIndex)
    {
        std::string bufferName = m_shaderSlots[slotIndex].GetBufferName();

        unsigned int priorityValue = GetSlotPriority(bufferName);
        int swapIndex = -1;

        for (unsigned int cmpSlotIndex = slotIndex + 1; cmpSlotIndex < m_shaderSlots.size(); ++cmpSlotIndex)
        {
            //Find priority of the current compare slot.
            std::string bufferName = m_shaderSlots[cmpSlotIndex].GetBufferName();

            unsigned int comparePrioritySlot = GetSlotPriority(bufferName);

            if (priorityValue > comparePrioritySlot)
            {
                swapIndex = cmpSlotIndex;
                priorityValue = comparePrioritySlot;
            }
        }

        if (swapIndex != -1)
        {
            //Swap the slots
            ShaderSlot slot = m_shaderSlots[slotIndex];
            m_shaderSlots[slotIndex] = m_shaderSlots[swapIndex];
            m_shaderSlots[swapIndex] = slot;
        }
    }
}

unsigned int XCShaderHandle::GetSlotPriority(std::string bufferName)
{
    for (unsigned int bufferIndex = 0; bufferIndex < gs_slotPriority.size(); ++bufferIndex)
    {
        if (strcmp(bufferName.c_str(), gs_slotPriority[bufferIndex].c_str()) == 0)
        {
            return bufferIndex;
        }
    }

    XCASSERT(false);
    Logger("Priority not set for the slot %s. Last will be set", bufferName);

    return gs_slotPriority.size();
}

void XCShaderHandle::ApplyShader(ID3DDeviceContext& context, RasterType rasterType)
{
#if defined(XCGRAPHICS_DX12)

#if defined(LOG_SHADER_SLOTS)
    Logger("[Apply Shader] %s", m_vertexShaderPath.c_str());
#endif

    m_pso->SetGraphicsPipelineState(context, rasterType);

#elif defined(XCGRAPHICS_DX11)
    context.IASetInputLayout(m_inputLayout);
    context.VSSetShader((ID3DVertexShader*) m_pVS, nullptr, 0);
    context.PSSetShader((ID3DPixelShader*)  m_pPS, nullptr, 0);
#endif
}


void XCShaderHandle::SetConstantBuffer(std::string bufferName, ID3DDeviceContext& context, D3DConstantBuffer& constantBuffer)
{
    auto bufferRes = std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(), 
        [&bufferName](ShaderSlot slot) -> bool 
    { 
        return strcmp(bufferName.c_str(), slot.GetBufferName().c_str()) == 0;
    });

    if (bufferRes != m_shaderSlots.end())
    {
        unsigned int slotNb = bufferRes - m_shaderSlots.begin();

#if defined(XCGRAPHICS_DX12)
        context.SetGraphicsRootDescriptorTable(slotNb, constantBuffer.m_gpuHandle);
#elif defined(XCGRAPHICS_DX11)
        context.VSSetConstantBuffers(slotNb, 1, &constantBuffer.m_gpuHandle);
        context.PSSetConstantBuffers(slotNb, 1, &constantBuffer.m_gpuHandle);
#endif

#if defined(LOG_SHADER_SLOTS)
        Logger("[SLOT] Set %s @ slot %d", bufferName.c_str(), slotNb);
#endif
        return;
    }

    XCASSERT(false);
}

void XCShaderHandle::SetSampler(std::string bufferName, ID3DDeviceContext& context, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    auto bufferRes = std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(),
        [&bufferName](ShaderSlot slot) -> bool
    {
        return strcmp(bufferName.c_str(), slot.GetBufferName().c_str()) == 0;
    });

    if (bufferRes != m_shaderSlots.end())
    {
        unsigned int slotNb = bufferRes - m_shaderSlots.begin();

#if defined(XCGRAPHICS_DX12)
        context.SetGraphicsRootDescriptorTable(slotNb, gpuHandle);
#elif defined(XCGRAPHICS_DX11)
        context.VSSetSamplers(slotNb, 1, nullptr);
        context.PSSetSamplers(slotNb, 1, nullptr);
#endif

#if defined(LOG_SHADER_SLOTS)
        Logger("[SLOT] Set %s @ slot %d", bufferName.c_str(), slotNb);
#endif
        return;
    }

    //Do not assert as some shaders do not contain samplers.
    //XCASSERT(false);
}

void XCShaderHandle::SetResource(std::string bufferName, ID3DDeviceContext& context, ResourceHandle* tex)
{
    auto bufferRes = std::find_if(m_shaderSlots.begin(), m_shaderSlots.end(),
        [&bufferName](ShaderSlot slot) -> bool
    {
        return strcmp(bufferName.c_str(), slot.GetBufferName().c_str()) == 0;
    });

    if (tex && bufferRes != m_shaderSlots.end())
    {
        unsigned int slotNb = bufferRes - m_shaderSlots.begin();

        switch (tex->m_Resource->GetResourceType())
        {
        case RESOURCETYPE_TEXTURE2D:
#if defined(XCGRAPHICS_DX12)
            context.SetGraphicsRootDescriptorTable(slotNb, static_cast<Texture2D*>(tex->m_Resource)->GetTextureResource()->m_gpuHandle);
#elif defined(XCGRAPHICS_DX11)
            context.VSSetShaderResources(slotNb, 1, &static_cast<Texture2D*>(tex->m_Resource)->GetTextureResource()->m_cbResource);
            context.PSSetShaderResources(slotNb, 1, &static_cast<Texture2D*>(tex->m_Resource)->GetTextureResource()->m_cbResource);
#endif
            break;

        case RESOURCETYPE_CUBETEXTURE3D:
#if defined(XCGRAPHICS_DX12)
            context.SetGraphicsRootDescriptorTable(slotNb, static_cast<CubeTexture3D*>(tex->m_Resource)->GetTextureResource()->m_gpuHandle);
#elif defined(XCGRAPHICS_DX11)
            context.VSSetShaderResources(slotNb, 1, &static_cast<CubeTexture3D*>(tex->m_Resource)->GetTextureResource()->m_cbResource);
            context.PSSetShaderResources(slotNb, 1, &static_cast<CubeTexture3D*>(tex->m_Resource)->GetTextureResource()->m_cbResource);
#endif
            break;

        default:
            XCASSERT(false);
        }

#if defined(LOG_SHADER_SLOTS)
        Logger("[SLOT] Setting %s @ slot %d", bufferName.c_str(), slotNb);
#endif
        return;
    }

    XCASSERT(false);
}
