@echo off
:: This is the Windows build script
:: If you are on Linux, see build.sh

$CoreLibraryDependencies=@("SDL.lib")

pushd build
cl /Wall main.c /link $CoreLibraryDependencies
popd
