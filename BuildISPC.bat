@echo off

set ispc="C:\Projects\ispc-v1.22.0-windows\bin\ispc.exe"

rem cd C:\Projects\xLights\xLights
 for /R xLights %%f in (*.ispc) do (
	@echo Found %%~nxf
	%ispc% "%%f" -h "%%f.h" --target avx2-i32x8
	%ispc% "%%f" -o "C:\Projects\xLights\xLights\x64\Debug\%%~nxf.obj" --target avx2-i32x8
	%ispc% "%%f" -o "C:\Projects\xLights\xLights\x64\Release\%%~nxf.obj" --target avx2-i32x8 -O2
 )