project "XCPhysics"
print("Building C++ Win32 Project XCPhysics")
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
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Physics;" }
		
		includedirs { "$(ProjectDir)Libs" }
		libdirs { "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", 
		"**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { }
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS", (GraphicsDefines) }
		pchheader "PhysicsPrecompiledHeader.h"
		pchsource "PhysicsPrecompiledHeader.cpp"
		
		symbols "On"
		buildoptions { "/Zo" }
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Physics;" }
		
		includedirs { "$(ProjectDir)Libs" }
		libdirs { "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", 
		"**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS", (GraphicsDefines) }
		pchheader "PhysicsPrecompiledHeader.h"
		pchsource "PhysicsPrecompiledHeader.cpp"
		
		symbols "On"
		
		optimize "On"
		buildoptions { "/Zo" }
				
		targetdir "$(ProjectDir)bin/$(Configuration)/"
				
print("Project Generation done...")