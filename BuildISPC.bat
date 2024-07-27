@echo off

set ispc="C:\Projects\ispc-v1.22.0-windows\bin\ispc.exe"

rem cd C:\Projects\xLights\xLights

echo Generating headers...
for /R xLights %%f in (*.ispc) do (
	@echo %%~nxf
	%ispc% "%%f" -h "%%f.h" --target avx2-i32x8
)

echo Compiling...
for /R xLights %%f in (*.ispc) do (
	@echo %%~nxf
	%ispc% "%%f" -o "C:\Projects\xLights\xLights\x64\Debug\%%~nxf.obj" --target avx2-i32x8 -g
	%ispc% "%%f" -o "C:\Projects\xLights\xLights\x64\Release\%%~nxf.obj" --target avx2-i32x8 -O2 -g
)