@ECHO off
echo This script accepts 1 parameter
echo  1 serial number of the aardvark pod in use, 0 if only 1
echo.

set ser=%1

if "x%ser%x" == "xx" set ser=0

if exist firmware\m2m_image_3400.bin (
	set CHPFAM=3400
)
if exist firmware\m2m_image_3A0.bin (
	set CHPFAM=3A0
)

:: default everything else
echo calling: download_all.bat I2C %CHPFAM% %ser% 0
echo.
download_all.bat I2C %CHPFAM% %ser% 0
