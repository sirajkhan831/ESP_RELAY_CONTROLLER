@echo off
setlocal

cd /d "%~dp0"
title BLYNK_PROJECT UI Server
echo Starting local UI launcher server...
echo Keep this window open. Closing it will stop the UI server.
start "" http://127.0.0.1:8765

where python >nul 2>&1
if %errorlevel%==0 goto RUN_SERVER_PYTHON

where py >nul 2>&1
if %errorlevel%==0 goto RUN_SERVER_PY

echo Python not found. Trying to install Python using winget...
where winget >nul 2>&1
if not %errorlevel%==0 (
  echo winget is not available on this system.
  echo Install Python manually from https://www.python.org/downloads/ and rerun this file.
  pause
  exit /b 1
)

winget install -e --id Python.Python.3.12 --accept-package-agreements --accept-source-agreements
if errorlevel 1 (
  echo Python installation failed. Please install manually and rerun this file.
  pause
  exit /b 1
)

where python >nul 2>&1
if %errorlevel%==0 goto RUN_SERVER_PYTHON

where py >nul 2>&1
if %errorlevel%==0 goto RUN_SERVER_PY

echo Python was installed but not found in PATH yet.
echo Please restart terminal/PC and run this file again.
pause
exit /b 1

:RUN_SERVER_PY
py -3 "ui\server.py"
if errorlevel 1 (
  echo py launcher failed. Trying python command...
  goto RUN_SERVER_PYTHON
)
goto END

:RUN_SERVER_PYTHON
python "ui\server.py"
if errorlevel 1 (
  echo python command failed. If Python was just installed, restart terminal/PC and rerun.
)

:END
echo.
echo UI server stopped.
pause
endlocal
