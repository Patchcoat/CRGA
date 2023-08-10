@echo off
:: This is the Windows build script
:: If you are on Linux, see build.sh

pushd build
cl /Wall ..\main.c
popd
