print("XC Framework Premake(Lua) Project Generation")

workspace "XCFramework"
	configurations { "Debug", "Release" }
	

project "XCFramework"
print("Building C++ Win32 Project")
	kind "WindowedApp"
	language "C++"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	platformVersion "10.0.10586.0"
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir)" }
		
		includedirs { "$(ProjectDir)", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/" }
		libdirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/Assimp" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "d3d12", "d2d1", "dwrite", "d3dcompiler", "FlatBuffers", "ws2_32", "DirectXTex", "dxguid", "dinput8", "dxgi", "d3d11", "glut32", "glew32", "assimp.lib","zlibstaticd.lib" }
		
		entrypoint("")
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "stdafx.h"
		pchsource "./Game/stdafx.cpp"
		
		flags { "Symbols" }
		
		postbuildcommands { "PostBuildProject.bat $(Configuration)" }
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir)" }
		
	    includedirs { "$(ProjectDir)", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/" }
	    libdirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers",  "$(ProjectDir)Libs/Assimp" }
	  
	    excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
				
	  	links { "d3d12", "d2d1", "dwrite", "d3dcompiler", "FlatBuffers", "ws2_32", "DirectXTex", "dxguid", "dinput8", "dxgi", "d3d11", "glut32", "glew32", "assimp.lib","zlibstaticd.lib" }
	  
		entrypoint("")
			
      	defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
	  	pchheader "stdafx.h"
		pchsource "./Game/stdafx.cpp"
      	
		optimize "On"
		
		postbuildcommands { "PostBuildProject.bat $(Configuration)" }
				
print("Project Generation done...")