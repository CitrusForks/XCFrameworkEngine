print("XC Framework Premake(Lua) Project Generation")

workspace "XCFramework"
	configurations { "Debug", "Release" }
	
print("Building C++ Win32 Project Game")
include "Game"	
print("Project Generation done...")

print("Building C++ Win32 Project XCBase")
include "Base"
print("Project Generation done...")

print("Building C++ Win32 Project XCEngine")
include "Engine"
print("Project Generation done...")

print("Building C++ Win32 Project XCGraphics")
include "Graphics"
print("Project Generation done...")

print("Building C++ Win32 Project XCNetwork")
include "Network"
print("Project Generation done...")

print("Building C++ Win32 Project XCGameplay")
include "Gameplay"

print("Building C++ UniTest Project XCUnitTest")
include "UnitTests"

print("Project Generation done...")