Language = "C++"
Architecture = "x86"
Targetdir = "bin/%{cfg.buildcfg}"
Characterset = "MBCS"

if _ACTION == "vs2017" then
    PlatformVersion = "10.0.15063.0"
elseif _ACTION == "vs2015" then
    PlatformVersion = "10.0.10586.0"
else 
    print("VS Platform not supported")
end

print("Using PlatformVersion " .. PlatformVersion)  

if _OPTIONS["gfxapi"] == "dx11" then
    GraphicsDefines = { "XCGRAPHICS_DX11" }
    GraphicsLibs = { "d3d11", "d3dcompiler", "DirectXTex", "dxguid", "dxgi" }
elseif _OPTIONS["gfxapi"] == "dx12" then
    GraphicsDefines = { "XCGRAPHICS_DX12" }
    GraphicsLibs = { "d3d12", "d3dcompiler", "DirectXTex", "dxguid", "dxgi" }
elseif _OPTIONS["gfxapi"] == "opengl" then
    GraphicsDefines = { "XCGRAPHICS_GL" }
    GraphicsLibs = { "glut32", "glew32" }
else
    print("Unsupported Graphics: Defaulting to dx11")
    GraphicsLibs = { "d3d11", "d3dcompiler", "DirectXTex", "dxguid", "dxgi" }
end

print("Using Graphics API " .. _OPTIONS["gfxapi"])
