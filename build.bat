@echo off
:: This is the Windows build script
:: If you are on Linux, see build.sh

:: change this to x64 to get the 64 bit version
set xlibs="x86"

pushd build
cl /Wall /Tc ..\main.c /I "..\SDL\include" /I "..\GLEW\include" /link "SDL2.lib" "SDL2main.lib" "glew32.lib" "opengl32.lib" "advapi32.lib" "shell32.lib" "user32.lib" /LIBPATH:"..\SDL\lib\\%xlibs%" /LIBPATH:"..\GLEW\lib\\%xlibs%"  /SUBSYSTEM:WINDOWS
>> "SDL2.dll" rem/
xcopy /y ..\SDL\lib\%xlibs%\SDL2.dll .\SDL2.dll
popd
