@ECHO off
set UTOOL=%1
set UTGTIN=%2
set UPRTIN=%3

echo Mode %UTOOL%
if /I "%UTOOL%" == "UART"  Goto contine_UART
if /I "%UTOOL%" == "I2C"   Goto contine_I2C
goto usage

:contine_I2C
set uptool=firmware\winc_programmer_i2c.exe
goto checkdevice

:contine_UART
set uptool=firmware\winc_programmer_uart.exe  -p \\.\COM%UPRTIN%
goto checkdevice

:checkdevice
echo Chip %UTGTIN%
if /I "%UTGTIN%" == "3400"  Goto chip3400
if /I "%UTGTIN%" == "3A0"   Goto chip3A0
if /I "%UTGTIN%" == "1500"  Goto chip3A0
goto usage

:chip3A0
set UVARIANT=3A0
set UCHPFAM=1500
set UPFW=programmer_release_text.bin
goto START

:chip3400
set UVARIANT=3400
set UCHPFAM=3400
set UPFW=programmer_firmware.bin
goto START

:START

echo Checking for Python support:
where /q python.exe
if ERRORLEVEL 1 GOTO NOPYTHON
goto :HASPYTHON
:NOPYTHON
echo Require Python v2.x
exit /b -1
:HASPYTHON
where python.exe
echo OK
echo.


echo Updating PLL table in image for %UVARIANT% variant
echo %uptool% -d winc%UCHPFAM%  -r efuse -pfw programmer_firmware\release%UVARIANT%\%UPFW%

%uptool% -d winc%UCHPFAM%  -r efuse -pfw programmer_firmware\release%UVARIANT%\%UPFW% > efuse_tmp

if not ERRORLEVEL 0 GOTO failure

python extract_xo_offset.py < efuse_tmp > xo_tmp

set /p xo_off= < xo_tmp
del xo_tmp
del efuse_tmp

echo XO Offset is %xo_off%
echo firmware\image_tool.exe -c firmware\flash_image.config -c Tools\gain_builder\gain_sheets\new_gain.config -o firmware\m2m_image_%UVARIANT%.bin -of prog  -cs "[pll table]\r\nfrequency offset is %xo_off%"

firmware\image_tool.exe -c firmware\flash_image.config  -c Tools\gain_builder\gain_sheets\new_gain.config  -cs "[pll table]\r\nfrequency offset is %xo_off%\r\n"  -o firmware\m2m_image_%UVARIANT%.bin -of prog 

firmware\image_tool.exe -c firmware\flash_image.config  -c Tools\gain_builder\gain_sheets\new_gain.config  -cs "[pll table]\r\nfrequency offset is %xo_off%\r\n"  -o firmware\m2m_image_%UVARIANT%.aoi -of raw >NUL

if not ERRORLEVEL 0 GOTO failure

set xo_off=
goto done

:USAGE
echo Usage %0 (I2C or UART) (1500 or 3400) (comport or 0 for I2C)
@ECHO %CMDCMDLINE% | FIND /I /C "/C" > NUL && PAUSE
exit /b -1

:failure
echo Unsuccessful
set xo_off=
exit /b -1

:done
echo Successful
