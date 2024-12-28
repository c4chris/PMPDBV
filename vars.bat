@echo off

set ROOTDIR=%~dp0
set PROJECT_DIR=%ROOTDIR:~0,-1%

set PROJECT=PMPDBV
set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT%.uproject

::set UE5_DIR=D:\UnrealEngine
set UE5_DIR=C:\Program Files\Epic Games\UE_5.5\
set UE5EDITOR_EXE=%UE5_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set BUILD_BAT=%UE5_DIR%\Engine\Build\BatchFiles\Build.bat
