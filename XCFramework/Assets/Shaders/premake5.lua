print("XC Framework Shaders Premake(Lua) Project Generation")

workspace "XCFrameworkShaders"
	configurations { "Debug", "Release" }
	
project "XCShaders"
print("Building C Shader project")
	kind "StaticLib"
	language "C"
	architecture "x86"
	targetdir "bin/%{cfg.buildcfg}"
	systemversion "10.0.10586.0"
	characterset "MBCS"
	
	files { "**.hlsl", "**.licenseheader" }
	
	--Configuration Debug
	print("Building Debug")
	filter "configurations:Debug"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir);" }
		
		excludes { "**.pssl", "**.cso" }
		symbols "On"
		
		targetdir "$(ProjectDir)bin/$(Configuration)/"
		
	--Configuration Release
	print("Building Release")
	filter "configurations:Release"
		sysincludedirs {"$(VC_IncludePath);$(WindowsSDK_IncludePath);$(ProjectDir);$(ProjectDir);" }
		
		excludes { "**.pssl", "**.cso" }
		
		symbols "On"
      	
		optimize "On"
				
		targetdir "$(ProjectDir)bin/$(Configuration)/"

print("Project Generation done...")