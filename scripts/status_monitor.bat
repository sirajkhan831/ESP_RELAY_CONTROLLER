@echo off
setlocal

echo Opening PlatformIO serial monitor at 115200...
start "PIO Monitor" cmd /k pio device monitor -b 115200

endlocal
