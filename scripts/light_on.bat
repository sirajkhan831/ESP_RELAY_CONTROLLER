@echo off
setlocal

set "BAUD=115200"
set "CMD=RELAY ON"

echo Detecting ESP serial port via PlatformIO and sending "%CMD%"...
powershell -NoProfile -ExecutionPolicy Bypass -Command "$cmd='%CMD%';$baud=%BAUD%;$devices=pio device list --json-output | ConvertFrom-Json;$esp=$devices | Where-Object{$_.hwid -match 'VID:PID=(10C4:EA60|1A86:7523|0403:6001)'} | Select-Object -First 1;if(-not $esp){Write-Error 'No ESP USB-UART port found. Connect board and retry.';exit 1};$port=$esp.port;Write-Host ('Using port: ' + $port);$p=New-Object System.IO.Ports.SerialPort($port,$baud,'None',8,'one');try{$p.Open();Start-Sleep -Milliseconds 300;$p.DiscardInBuffer();$p.WriteLine($cmd);Start-Sleep -Milliseconds 250;$resp=$p.ReadExisting();if($resp -match '\[BLOCKED\]'){Write-Host 'Cloud mode active: local relay scripts are disabled. Run mode_offline.bat first.';exit 2}elseif($resp -match '\[RELAY\]'){Write-Host 'Relay command executed successfully.'}else{Write-Host 'Command sent, but no confirmation received.'}}catch{Write-Error $_;exit 1}finally{if($p -and $p.IsOpen){$p.Close()}}"

endlocal
