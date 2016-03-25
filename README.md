# XCFrameworkEngine
XCFramework is a cross platform Game engine written in C++ with DirectX12, DirectX11 and OpenGL at it's core for win32 platform. It also provides support for Xbox One and PS4. 

![alt tag](http://3.bp.blogspot.com/-iq8T8NVzv94/VNOlfA7k64I/AAAAAAAAFyg/cvNiCBVb-kc/s1600/xcframework_terrain.PNG)
![alt tag](http://3.bp.blogspot.com/-vQwAOfaZ4E4/VPH5n7pWfxI/AAAAAAAAF2I/73GkEw2gHW0/s1600/XCFramework_Editor.PNG)
![alt tag](https://3.bp.blogspot.com/-689TitxiBwE/VvUpM6OY8dI/AAAAAAAAITg/06f0I7Dai-EYMg0gdI_QJFRrjCxF0VBDw/s640/Capture.PNG)


# Features
- Vector Font Instance Rendering.
- Offline Shader tools to build the shader binaries.
- Engine supports Reflection API, to fetch description of shaders and create respective layouts and slots. 
- VR support - Realtime rendering of frames from engine to a smartphone through network protocols (using socks) and provides Virtual Reality in a split screen that requires a Google Cardboard. Accelerometer inputs allows the user to move the objects in the engine. (Working with Dx11 enabled. With Dx12 to come soon.)
- XCFramework Editor - Runtime editor allows to edit the world, create levels, manage resources. (Non functional after Dx12 implementation. Stay tuned for this!)
- World Manager - an world architecture that includes actors such as Physics actors, Light actors, Animated actors and various other actors that this world can load into and act as a container of these actors.
- Terrain Generation 
- Custom Serializer - Runtime type deduction and object construction.
- FlatBuffer API from Google to serialize JSON objects which allows easy scripting of data such as textures, shaders and other game assets.
- .3ds Mesh Loader - to read .3ds files created from 3D modelling tools such as 3dsMax, Maya. It parses the .3ds files and creates a XC_Mesh which is engine custom format and renders the mesh in the scene.
- Assimp API to load the animated skinned mesh
- XC Physics Engine - Custom physics engine. Provides bounding 3D objects, collision detection and collision resolver.
- AI - Integrated basic AI system to Non Playable Characters.

# Dev Requirements
Windows 10 SDK 10.0.10586.0 
Directx12 supported hardware (If not, Warp will be enabled automatically by engine. But it might crash and is unstable).
Directx11 is broken. Wait for it!

# How to build the engine and run a 3D game?
Run "Premake5.exe vs2015" through commandline.
Launch the solution and build it.

# How to get data?
Not required. Its already in the repo.

# TODO Integrations of my other project
- Runtime reflection of shader changes without re-launching the engine.
- Include Motion Tracker to track user gestures and provide input system within the engine.
- Provide drag and drop of assets into the world and including within the package.

[Note : Custom Premake5 built included. Do not use other premake, as the master premake doesn't allow specifying SDK version. Check the Premake5.lua which includes a attribute "platformVersion" which I have added the fucntionality into this version.]
