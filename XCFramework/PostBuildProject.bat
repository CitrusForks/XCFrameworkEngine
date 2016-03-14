echo "Copying Files to bin directory"
robocopy .\Libs\Assimp\ .\bin\%1\ assimp.dll

echo "Compiling shaders"
.\Assets\Shaders\XCShaderCompiler.exe .\Assets\Shaders\