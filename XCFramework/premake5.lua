print("XC Framework Premake(Lua) Project Generation")

newoption {
   trigger     = "gfxapi",
   value       = "API",
   description = "Choose a particular 3D API for rendering",
   allowed = {
      { "opengl",    "OpenGL" },
      { "dx11",  "Direct3D 11 (Windows only)" },
      { "dx12",  "Direct3D 12 (Windows only)" }
   }
}

workspace "XCFramework"
	configurations { "Debug", "Release" }
	
include "CommonCfg"

group("Engine")
print("Building C++ Win32 Project XCBase")
include "Base"
print("Project Generation done...")

print("Building C++ Win32 Project XCEngine")
include "Engine"
print("Project Generation done...")

print("Building C++ Win32 Project XCPhysics")
include "Physics"
print("Project Generation done...")

print("Building C++ Win32 Project XCGraphics")
include "Graphics"
print("Project Generation done...")

group("Gameplay")
print("Building C++ Win32 Project XCGameplay")
include "Gameplay"
print("Project Generation done...")

group("Network")
print("Building C++ Win32 Project XCNetwork")
include "Network"
print("Project Generation done...")

group("Exe")
print("Building C++ Win32 Project XCGame")
include "Game"	
print("Project Generation done...")

group("Test")
print("Building C++ UniTest Project XCUnitTest")
include "UnitTests"
print("Project Generation done...")

print("Building C Shader Solution ")
include "Assets/Shaders/"
print("Solution Generation done...")
