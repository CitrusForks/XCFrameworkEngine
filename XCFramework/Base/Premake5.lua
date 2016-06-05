project "XCBase"
print("Building C++ Win32 Project XCBase")
	kind "StaticLib"
	language "C++"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	platformVersion "10.0.10586.0"
	
	files { "**.h", "**.cpp", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Base;" }
		
		includedirs { "$(ProjectDir)Libs/" }
		libdirs { }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "d3d12", "ws2_32", "DirectXTex", "dxgi", "d3d11" }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "BasePrecompiledHeader.h"
		pchsource "BasePrecompiledHeader.cpp"
		
		flags { "Symbols" }
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir)Base;" }
		
		includedirs { }
		libdirs { }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "d3d12", "ws2_32", "DirectXTex", "dxgi", "d3d11" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
		pchheader "BasePrecompiledHeader.h"
		pchsource "BasePrecompiledHeader.cpp"
		
		flags { "Symbols" }
      	
		optimize "On"
				
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
print("Project Generation done...")