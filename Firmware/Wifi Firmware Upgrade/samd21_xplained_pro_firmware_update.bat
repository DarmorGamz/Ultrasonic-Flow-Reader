@ECHO Off
pushd firmware
set varPath=%PROGRAMFILES%
set PythonVersion=C:\Python27
set PATH=%PythonVersion%;%PATH%

echo usage:
echo   This program will program a WINC1500 Xplained card plugged into a SamD21XplainedPro card, so long as only one is present
echo   It fills in defaults and calls more specific script files below.
echo.

echo Checking for Python support.
where /q python.exe
if %ERRORLEVEL%==1 GOTO NOPYTHON
python --version > tmpFile 2>&1
set /p myvar= < tmpFile 
del tmpFile
echo python version = %myvar%
echo.%myvar% | FINDSTR /C:"2."
echo error level = %ERRORLEVEL%
if %ERRORLEVEL%==0 GOTO HASPYTHON
:NOPYTHON
echo This tool require Python v2.x. Kindly change the path of Python version details in the variable "PythonVersion" of bat file "samd21_xplained_pro_firmware_update.bat" .
popd
exit /b 2
:HASPYTHON
echo Python 2.x available

:CheckOS
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO RUN)
:64BIT
set varPath=%PROGRAMFILES(X86)%
:RUN


set /A edbgCnt=0
set SN=0
for /f "tokens=1-2" %%i in ('"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" list') do (
	if "%%i" == "edbg" (
		set SN=%%j
		set /A edbgCnt+=1
	)
)	

if %edbgCnt%==0 (
	echo Cannot find and EDBG boards?
	echo see  '"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" list'
	exit /b 1
)

if %edbgCnt% GTR 1 (
	echo This batch file is unsuitable if more than one EDBG based development board is installed, found %edbgCnt%
	echo Use download_all_sb.bat with options
	echo		edbg 
	echo		ATSAMD21J18A 
	echo		Tools\serial_bridge\samd21_xplained_pro_serial_bridge.elf 
	echo		3400 or 3A0 [1500/1510]
	echo		serial number of the dev board attached to the board you wish to program - see '"%varPath%\Atmel\Studio\7.0\atbackend\atprogram.exe" list'
	echo		com port number assigned to the dev board attached to the board you wish to program by the OS
	exit /b 1
)

:: On the apps system the files are in a firmware folder, for dev they are at repo root.
if exist download_all_sb.bat (
  pushd .
) else (
  pushd firmware
)

if not exist download_all_sb.bat (
  echo File layout error
  popd
  pause
  exit /b 1
)

set CHPFAM=error
if exist firmware\m2m_image_3400.bin (
	if not exist firmware\m2m_image_3A0.bin set CHPFAM=3400
)
if exist firmware\m2m_image_3A0.bin (
	if not exist firmware\m2m_image_3400.bin set CHPFAM=3A0
)

if %CHPFAM%==error (
	echo Must have one of firmware\m2m_image_3A0.bin and firmware\m2m_image_3400.bin
	echo Try running Prepare_image.cmd
	popd
	pause
	exit /b 1
)


echo download_all_sb.bat edbg ATSAMD21J18A Tools\serial_bridge\samd21_xplained_pro_serial_bridge.elf %CHPFAM% %SN% 0
call download_all_sb.bat edbg ATSAMD21J18A Tools\serial_bridge\samd21_xplained_pro_serial_bridge.elf %CHPFAM% %SN% 0

popd
pause
