@echo off
title 2D Car Parking Simulation Launcher
setlocal

set "HTML_PATH=%~dp0index.html"
set "HTML_URL=file:///%HTML_PATH:\=/%"

echo ========================================================
echo   Launching 2D Car Parking Simulation (Official Edition)
echo ========================================================
echo.

:: 1. Try launching in dedicated window mode using Microsoft Edge (Signed by Microsoft, Never blocked by Smart App Control)
if exist "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" (
    start "" "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" --app="%HTML_URL%" --window-size=1020,800
    exit /b 0
)

if exist "C:\Program Files\Microsoft\Edge\Application\msedge.exe" (
    start "" "C:\Program Files\Microsoft\Edge\Application\msedge.exe" --app="%HTML_URL%" --window-size=1020,800
    exit /b 0
)

:: 2. Try Google Chrome App Mode
if exist "C:\Program Files\Google\Chrome\Application\chrome.exe" (
    start "" "C:\Program Files\Google\Chrome\Application\chrome.exe" --app="%HTML_URL%" --window-size=1020,800
    exit /b 0
)

:: 3. Default browser fallback
start "" "%HTML_PATH%"
exit /b 0
