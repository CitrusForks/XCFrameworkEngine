@echo Generating XCFramework solutions .... 
@echo off
premake5.exe vs2017 --gfxapi="dx12"
@echo on
pause