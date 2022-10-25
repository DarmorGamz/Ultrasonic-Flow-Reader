@ECHO off
setlocal EnableDelayedExpansion

:: %1 type of bus
:: %2 (3A0 or 1500 or 3400) 
:: %3 (aardvark_serial_number or 0)
:: %4 com port for edbg usb device or 0

set TOOL=%1
set TGTIN=%2
set AADVSNIN=%3
set PRTIN=%4

echo Mode %TOOL%
if /I "%TOOL%" == "UART"  Goto contine_UART
if /I "%TOOL%" == "I2C"   Goto contine_I2C

goto usage

:contine_I2C
Set  FMode=debug_i2c
set Mode=I2C
set ptool=winc_programmer_i2c.exe
goto START

:contine_UART
Set  FMode=debug_uart
set Mode=UART
set ptool=winc_programmer_uart.exe  -p \\.\COM%PRTIN%
goto START

:START

if /I "a%AADVSNIN%a" == "aa" (
	set AARDVARK=0
) else (
	set AARDVARK=%AADVSNIN%
)

echo Chip %TGTIN%
if /I "%TGTIN%" == "3400"  Goto chip3400
if /I "%TGTIN%" == "3A0"   Goto chip3A0
if /I "%TGTIN%" == "1500"  Goto chip3A0

:USAGE
echo Usage %0 (I2C-UART) (3A0 or 1500 or 3400) (aardvark_serial_number or 0) (comport or 0 for I2C)
@ECHO %CMDCMDLINE% | FIND /I /C "/C" > NUL && PAUSE
exit /b 2

:chip3A0
set VARIANT=3A0
set CHPFAM=1500
set PFW=programmer_release_text.bin
goto parmsok

:chip3400
set VARIANT=3400
set CHPFAM=3400
set PFW=programmer_firmware.bin
goto parmsok

:parmsok

call update_pll_table.bat %TOOL% %CHPFAM% %PRTIN%

if not ERRORLEVEL 0 GOTO FAILED

pushd firmware
echo Downloading Image...

:: winc_programmer_uart.exe -d wincXX00 -e -i m2m_image_XX00.bin -if prog -w -r -p \\.\COM73 -pfw ..\programmer_firmware\releaseXX00\programmer_firmware.bin
echo Call %ptool% -d winc%CHPFAM% -i m2m_image_%VARIANT%.bin -if prog -e -w -r -pfw ..\programmer_firmware\release%VARIANT%\%PFW%
%ptool% -d winc%CHPFAM% -i m2m_image_%VARIANT%.bin -if prog -e -w -r -pfw ..\programmer_firmware\release%VARIANT%\%PFW%
if %ERRORLEVEL% NEQ 0 GOTO FAILED  
goto SUCCESS

:FAILED
popd
echo *************** Failed to download *****************
@ECHO %CMDCMDLINE% | FIND /I /C "/C" > NUL && PAUSE
exit /b 1

:SUCCESS
popd
echo *************** Download worked *****************
@ECHO %CMDCMDLINE% | FIND /I /C "/C" > NUL && PAUSE
exit /b 0
