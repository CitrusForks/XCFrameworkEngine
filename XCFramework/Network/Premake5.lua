project "XCNetwork"
print("Building C++ Win32 Project XCNetwork")
	kind "StaticLib"
	language "C++"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	systemversion "10.0.10586.0"
	characterset "MBCS"
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Network;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers" }
		libdirs { "$(ProjectDir)Libs" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", 
		"**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "ws2_32" }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "NetworkPrecompiledHeader.h"
		pchsource "NetworkPrecompiledHeader.cpp"
		
		symbols "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Network;" }
		
		includedirs { "$(ProjectDir)Libs","$(ProjectDir)Libs/flatbuffers" }
		libdirs { "$(ProjectDir)Libs" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", 
		"**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "ws2_32" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
		pchheader "NetworkPrecompiledHeader.h"
		pchsource "NetworkPrecompiledHeader.cpp"
		
		symbols "On"
      	
		optimize "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
				
print("Project Generation done...")