/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifdef _MSC_VER
#pragma once
#endif


//#include <d3d11.h>

#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#pragma warning(pop)

namespace DirectX
{
    HRESULT CreateDDSTextureFromMemory( _In_ ID3D11Device* d3dDevice,
                                        _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
                                        _In_ size_t ddsDataSize,
                                        _Out_opt_ ID3D11Resource** texture,
                                        _Out_opt_ ID3D11ShaderResourceView** textureView,
                                        _In_ size_t maxsize = 0
                                      );

    HRESULT CreateDDSTextureFromFile( _In_ ID3D11Device* d3dDevice,
                                      _In_z_ const wchar_t* szFileName,
                                      _Out_opt_ ID3D11Resource** texture,
                                      _Out_opt_ ID3D11ShaderResourceView** textureView,
                                      _In_ size_t maxsize = 0
                                    );

    HRESULT CreateDDSTextureFromMemoryEx( _In_ ID3D11Device* d3dDevice,
                                          _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
                                          _In_ size_t ddsDataSize,
                                          _In_ size_t maxsize,
                                          _In_ D3D11_USAGE usage,
                                          _In_ unsigned int bindFlags,
                                          _In_ unsigned int cpuAccessFlags,
                                          _In_ unsigned int miscFlags,
                                          _In_ bool forceSRGB,
                                          _Out_opt_ ID3D11Resource** texture,
                                          _Out_opt_ ID3D11ShaderResourceView** textureView
                                      );

    HRESULT CreateDDSTextureFromFileEx( _In_ ID3D11Device* d3dDevice,
                                        _In_z_ const wchar_t* szFileName,
                                        _In_ size_t maxsize,
                                        _In_ D3D11_USAGE usage,
                                        _In_ unsigned int bindFlags,
                                        _In_ unsigned int cpuAccessFlags,
                                        _In_ unsigned int miscFlags,
                                        _In_ bool forceSRGB,
                                        _Out_opt_ ID3D11Resource** texture,
                                        _Out_opt_ ID3D11ShaderResourceView** textureView
                                    );
}