@echo off

set GPP="C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"
if not exist %GPP% set GPP=g++

set NASM="bin\nasm.exe"
if not exist %NASM% set NASM="C:\Users\Sumit\AppData\Local\bin\NASM\nasm.exe"
if not exist %NASM% set NASM="C:\Program Files\NASM\nasm.exe"
if not exist %NASM% set NASM=nasm

echo [COA] Assembling COA_Processor.asm with NASM (Win64 ABI)...
%NASM% -f win64 COA_Processor.asm -o COA_Processor_asm.o
if %ERRORLEVEL% NEQ 0 (
    echo [BUILD FAILED] NASM assembly failed.
    exit /b %ERRORLEVEL%
)

echo [G++] Compiling and linking C++ modules + COA_Processor_asm.o into 2D_Car_Parking.exe...
%GPP% -std=c++11 -Wall -O2 ^
    -I"include" ^
    main.cpp Game.cpp PL_DataStructures.cpp OOPS_Vehicle.cpp OOPS_Car.cpp ^
    OOPS_Obstacle.cpp OOPS_ParkingLot.cpp COA_Processor_asm.o CG_Renderer.cpp ^
    -o "2D_Car_Parking.exe" ^
    "freeglut.dll" -lopengl32 -lglu32 -lgdi32 -lwinmm

if %ERRORLEVEL% EQU 0 (
    echo [BUILD SUCCESS] 2D_Car_Parking.exe built successfully!
) else (
    echo [BUILD FAILED] Compilation errors occurred.
)
