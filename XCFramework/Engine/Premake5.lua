project "XCEngine"
print("Building C++ Win32 Project XCEngine")
	kind "StaticLib"
	language "C++"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	platformVersion "10.0.10586.0"
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Engine;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AdobeFBX/include/" }
		libdirs { "$(ProjectDir)Libs", "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/Assimp", "$(ProjectDir)Libs/AdobeFBX/x86/Debug/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "FlatBuffers", "ws2_32", "dinput8", "XCGraphics" }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "EnginePrecompiledHeader.h"
		pchsource "EnginePrecompiledHeader.cpp"
		
		flags { "Symbols" }
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
				sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Engine;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers" }
		libdirs { "$(ProjectDir)Libs", "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/Assimp", "$(ProjectDir)Libs/AdobeFBX/x86/Debug/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "FlatBuffers", "ws2_32", "dinput8", "XCGraphics" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
		pchheader "EnginePrecompiledHeader.h"
		pchsource "EnginePrecompiledHeader.cpp"
		
		flags { "Symbols" }
      	
		optimize "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
				
print("Project Generation done...")