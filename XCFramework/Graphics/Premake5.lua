project "XCGraphics"
print("Building C++ Win32 Project XCGraphics")
	kind "StaticLib"
	language (Language)
	architecture (Architecture)
	targetdir (Targetdir)
	systemversion (PlatformVersion)
	characterset (Characterset)
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Graphics/;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AutodeskFBX/include/" }
		libdirs { "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs", "$(ProjectDir)Libs/DirectXTex/$(Configuration)/", 
		"$(ProjectDir)Libs/flatbuffers/$(Configuration)", "$(ProjectDir)Libs/Assimp/$(Configuration)", 
		"$(ProjectDir)Libs/AutodeskFBX/x86/$(Configuration)/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", 
		"**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/src/**", "**Graphics/XC_Shaders/src/**"}
		
		links { (GraphicsLibs), "FlatBuffers" }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS", (GraphicsDefines) }
		pchheader "GraphicsPrecompiledHeader.h"
		pchsource "GraphicsPrecompiledHeader.cpp"
		
		symbols "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
		postbuildcommands { "PostBuildProject.bat $(Configuration)" }
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Graphics/;" }
		
		includedirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AutodeskFBX/include/" }
		libdirs { "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs", "$(ProjectDir)Libs/DirectXTex/$(Configuration)/", 
		"$(ProjectDir)Libs/flatbuffers/$(Configuration)", "$(ProjectDir)Libs/Assimp/$(Configuration)", 
		"$(ProjectDir)Libs/AutodeskFBX/x86/$(Configuration)/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", 
		"**/DDSTextureLoader_Legacy/**", "**.pssl", "**/src/**", "**Graphics/XC_Shaders/src/**"}
		
		links { (GraphicsLibs), "FlatBuffers" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS", (GraphicsDefines) }
		pchheader "GraphicsPrecompiledHeader.h"
		pchsource "GraphicsPrecompiledHeader.cpp"
		
		symbols "On"
		
		optimize "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
		postbuildcommands { "PostBuildProject.bat $(Configuration)" }
				
print("Project Generation done...")