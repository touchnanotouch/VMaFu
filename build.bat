@echo off
setlocal enabledelayedexpansion

title VMaFu Build System

echo.
echo ========================================
echo    VMaFu Build System
echo ========================================
echo.

:: Очистка предыдущей сборки
echo.
echo [1/4] Cleaning previous build...
if exist "build" (
    echo Removing old build directory...
    rmdir /s /q build
    echo [✓] Clean complete
) else (
    echo No previous build found
)

:: Создание структуры папок
echo.
echo [2/4] Creating directory structure...
mkdir build >nul 2>nul
mkdir build\obj >nul 2>nul
mkdir build\bin >nul 2>nul
mkdir build\lib >nul 2>nul
echo [✓] Directory structure created

:: CMake сборка
echo.
echo [3/4] CMake build...
echo ------------------------------------
where cmake >nul 2>nul
if not errorlevel 1 (
    echo [INFO] CMake found, generating build system...
    mkdir build 2>nul
    cd build
    cmake ..
    if not errorlevel 1 (
        echo.
        echo [INFO] Building with CMake...
        echo ------------------------------------
        cmake --build .
        echo ------------------------------------
        echo [✓] CMake build successful
    ) else (
        echo [✗] CMake configuration failed
    )
    cd ..
) else (
    echo [INFO] CMake not found, skipping CMake build
)

:: Вывод итога сборки
echo.
echo [4/4] Build summary
echo ====================================
echo.

if exist "build\bin\*.exe" (
    echo [SUCCESS] Build completed!
    echo.
    echo Available executables in build\bin\:
    echo ------------------------------------
    dir /b build\bin\*.exe
    echo.
) else if exist "build\*.exe" (
    echo [SUCCESS] Build completed!
    echo.
    echo Available executables in build\:
    echo ------------------------------------
    dir /b build\*.exe
    echo.
) else (
    echo [WARNING] No executables found after build
    echo.
)

echo Build process finished at %time%
echo.

pause