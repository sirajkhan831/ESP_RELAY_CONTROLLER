@echo off
setlocal

set "BLYNK_TOKEN=Yb3z84yZvG5RrDlzF1oPYfkQM7dHRGdM"
set "BLYNK_BASE_URL=https://blynk.cloud"

echo Sending CLOUD ON command...
curl -s "%BLYNK_BASE_URL%/external/api/update?token=%BLYNK_TOKEN%&V0=1" >nul
if errorlevel 1 (
  echo Failed to send cloud ON command.
  exit /b 1
)
echo Cloud ON command sent.

endlocal
