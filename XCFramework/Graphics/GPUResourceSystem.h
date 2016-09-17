/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#pragma once


#include "Graphics/RenderTargetTypes.h"
#include "GPUResourceType.h"

#include "Engine/System/ISystem.h"
#include "Engine/Thread/CriticalSection.h"

class GPUResource;
class GPUResourceView;

class GPUResourceSystem : public ISystem
{
public:
    DECLARE_OBJECT_CREATION(GPUResourceSystem)

    void                 Init(ID3DDevice& device);
    void                 Destroy();

    //Constant Buffers
    GPUResource*         CreateConstantBufferResource(GPUResourceDesc& desc);
    void                 CreateConstantBufferView(GPUResource* res);
    GPUResource*         CreateConstantBufferResourceView(GPUResourceDesc& desc);

    //Texture
    GPUResource*         CreateTextureResource(D3D_TEXTURE2D_DESC& textureDesc, const D3D_SUBRESOURCE_DATA* subResourceData);
    GPUResource*         CreateRenderTextureResource(ID3DSwapChain* swapChain, RenderTargetsType type);

    //RenderTargetsViews
    //void               CreateRenderTargetView(D3D_TEXTURE2D_DESC& desc, GPUResource* gpuResource);
    void                 CreateRenderTargetView(GPUResource* gpuResource);

    //ShaderResourceViews
    void                 CreateShaderResourceView(D3D_SHADER_RESOURCE_VIEW_DESC& viewDesc, GPUResource* gpuResource);

    //Destruction of resources
    void                 DestroyResource(GPUResource* buffer);

private:
    ID3DDevice*          m_device;
};