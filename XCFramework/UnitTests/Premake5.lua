project "XCUnitTest"
print("Building C++ Win32 Project XCUnitTest")
	kind "SharedLib"
	language "C++"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	platformVersion "10.0.10586.0"
	
	files { "**.h", "**.cpp", "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)UnitTests;" }
		
		includedirs { "$(VCInstallDir)UnitTest/include", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers" }
		libdirs { "$(VCInstallDir)UnitTest/lib;", "$(ProjectDir)Libs" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		defines { "DEBUG", "WIN_32", "WIN32", "_DEBUG", "_WINDOWS" }
		pchheader "UnitTestPrecompiledHeader.h"
		pchsource "UnitTestPrecompiledHeader.cpp"
		
		flags { "Symbols" }
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir)UnitTests;" }
		
		includedirs { "$(VCInstallDir)UnitTest/include", "$(ProjectDir)Libs", "$(ProjectDir)Libs/flatbuffers" }
		libdirs { "$(VCInstallDir)UnitTest/lib;", "$(ProjectDir)Libs" }

		excludes { "**/Orbis/**", "**orbis.cpp", "**/Durango/**", "**durango.cpp", "**editor.cpp", "**/Editor/**", "**Editor.cpp", "**Editor**.*", "**/DDSTextureLoader_Legacy/**", "**.pssl", "**/XC_Shaders/src/**"}
		
		links { "ws2_32" }
		
		defines { "NDEBUG", "WIN_32", "WIN32", "_WINDOWS" }
		pchheader "UnitTestPrecompiledHeader.h"
		pchsource "UnitTestPrecompiledHeader.cpp"
		
		flags { "Symbols" }
      	
		optimize "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
				
print("Project Generation done...")