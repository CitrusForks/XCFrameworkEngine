project "XCBase"
print("Building C++ Win32 Project XCBase")
	kind "StaticLib"
	language (Language)
	architecture (Architecture)
	targetdir (Targetdir)
	systemversion (PlatformVersion)
	characterset (Characterset)
	
	files { "**.h", "**.cpp", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Base;" }
		
		includedirs { "$(ProjectDir)Libs/" }
		libdirs { }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", 
		"**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "ws2_32" }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "BasePrecompiledHeader.h"
		pchsource "BasePrecompiledHeader.cpp"
		
		symbols "On"
		buildoptions { "/Zo" }
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Base;" }
		
		includedirs { "$(ProjectDir)Libs/" }
		libdirs { }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", 
		"**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "ws2_32" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
		pchheader "BasePrecompiledHeader.h"
		pchsource "BasePrecompiledHeader.cpp"
		
		symbols "On"
      	
		optimize "On"
		buildoptions { "/Zo" }
				
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
print("Project Generation done...")