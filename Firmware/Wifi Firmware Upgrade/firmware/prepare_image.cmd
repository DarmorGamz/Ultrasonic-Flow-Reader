@echo off

if a%1a==aa (
  echo ERROR Call with target chip 1500 or 3400 to build a compound and OTA image.
  exit /B 2
)

if a%1a==a3400a (
  set TGT_CHIP=3400
) else (
  :: Not too fussy, 3400 or default to 1500
  set TGT_CHIP=3A0
)

echo Converting gain table
echo Default gain table specified as Tools\gain_builder\gain_sheets\samd21_gain_setting_hp.csv
echo Change this script if different gain sheet is required!
echo python gain_converter.py Tools\gain_builder\gain_sheets\samd21_gain_setting_hp.csv -o new_gain.config
python gain_converter.py Tools\gain_builder\gain_sheets\atwinc1500_mr210pb_gain_setting.csv -o Tools\gain_builder\gain_sheets\new_gain.config

if not exist Tools\gain_builder\gain_sheets\new_gain.config (
  echo ERROR creating gain table config Tools\gain_builder\gain_sheets\new_gain.config
  exit /B 1
)

echo Building flash images for %TGT_CHIP% variant (prog and OTA format)

if exist firmware\m2m_image_%TGT_CHIP%.bin.bak del firmware\m2m_image_%TGT_CHIP%.bin.bak
if exist firmware\m2m_image_%TGT_CHIP%.bin ren firmware\m2m_image_%TGT_CHIP%.bin m2m_image_%TGT_CHIP%.bin.bak
firmware\image_tool.exe -c firmware\flash_image.config -c Tools\gain_builder\gain_sheets\new_gain.config -o firmware\m2m_image_%TGT_CHIP%.bin -of prog
if not exist firmware\m2m_image_%TGT_CHIP%.bin (
  echo ERROR creating firmware\m2m_image_%TGT_CHIP%.bin
  exit /B 1
)

if exist ota_firmware\m2m_ota_%TGT_CHIP%.bin.bak del ota_firmware\m2m_ota_%TGT_CHIP%.bin.bak
if exist ota_firmware\m2m_ota_%TGT_CHIP%.bin ren ota_firmware\m2m_ota_%TGT_CHIP%.bin m2m_ota_%TGT_CHIP%.bin.bak
firmware\image_tool.exe -c firmware\flash_image.config -c Tools\gain_builder\gain_sheets\new_gain.config -o ota_firmware\m2m_ota_%TGT_CHIP%.bin -of winc_ota -s ota
if not exist ota_firmware\m2m_ota_%TGT_CHIP%.bin (
  echo ERROR creating ota_firmware\m2m_ota_%TGT_CHIP%.bin
  exit /B 1
)

echo **********************************************************
echo Success
echo Created: firmware\m2m_image_%TGT_CHIP%.bin
echo Created: ota_firmware\m2m_ota_%TGT_CHIP%.bin
