echo "Copying Files to bin directory"
robocopy .\Libs\Assimp\ .\bin\%1\ assimp.dll
robocopy .\Libs\AutodeskFBX\x86\%1\ .\bin\%1\ libfbxsdk.dll

echo "Compiling shaders"
.\Assets\Shaders\XCShaderCompiler.exe .\Assets\Shaders\