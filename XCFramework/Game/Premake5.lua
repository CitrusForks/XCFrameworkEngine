project "XCGame"
print("Building C++ Win32 Project XCGame")
	kind "WindowedApp"
	language (Language)
	architecture (Architecture)
	targetdir (Targetdir)
	systemversion (PlatformVersion)
	characterset (Characterset)
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Game;" }
		
		includedirs { "$(ProjectDir)", "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs", 
		"$(ProjectDir)Libs/flatbuffers", "$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AutodeskFBX/include/" }
		libdirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/DirectXTex/$(Configuration)/", 
		"$(ProjectDir)Libs/flatbuffers/$(Configuration)/", "$(ProjectDir)Libs/Assimp/$(Configuration)/", "$(ProjectDir)Libs/AutodeskFBX/x86/$(Configuration)/" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { (GraphicsLibs), "FlatBuffers", "ws2_32", "dinput8", "assimp.lib","zlibstaticd.lib", "libfbxsdk.lib",
				"XCBase", "XCEngine", "XCGraphics", "XCNetwork", "XCGameplay", "XCPhysics" }
		
		entrypoint("")
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS", (GraphicsDefines) }
		pchheader "GamePrecompiledHeader.h"
		pchsource "GamePrecompiledHeader.cpp"
		
		symbols "On"
		buildoptions { "/Zo" }
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"

		postbuildcommands { "PostBuildProject.bat $(Configuration)"}
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)Game;" }
		
	    includedirs { "$(ProjectDir)", "$(ProjectDir)bin/$(Configuration)/", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers", 
		"$(ProjectDir)Libs/assimp/include/", "$(ProjectDir)Libs/AutodeskFBX/include/" }
	    libdirs { "$(ProjectDir)Libs", "$(ProjectDir)Libs/DirectXTex/$(Configuration)/", "$(ProjectDir)Libs/flatbuffers/$(Configuration)/", 
		"$(ProjectDir)Libs/Assimp/$(Configuration)/", "$(ProjectDir)Libs/AutodeskFBX/x86/$(Configuration)/" }
	  
	    excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", 
		"**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
				
	  	links { (GraphicsLibs), "FlatBuffers", "ws2_32", "dinput8", "assimp.lib","zlibstatic.lib", "libfbxsdk.lib",
			    "XCBase", "XCEngine", "XCGraphics", "XCNetwork", "XCGameplay",  "XCPhysics" }
	  
		entrypoint("")
			
      	defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS", (GraphicsDefines) }
	  	pchheader "GamePrecompiledHeader.h"
		pchsource "GamePrecompiledHeader.cpp"
      	
		symbols "On"
		
		optimize "On"
		buildoptions { "/Zo" }
				
		targetdir "$(ProjectDir)bin/$(Configuration)/"
				
		postbuildcommands { "PostBuildProject.bat $(Configuration)"}

print("Project Generation done...")