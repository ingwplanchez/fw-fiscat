for /f "delims=" %%a in ('wmic OS Get localdatetime  ^| find "."') do set dt=%%a
set YY=%dt:~2,2%
set MM=%dt:~4,2%
set DD=%dt:~6,2%
set HH=%dt:~8,2%
set Min=%dt:~10,2%

set stamp=%YY%%MM%%DD%_%HH%%Min%

stm32_crc32.exe SMART_KEYBOARD.bin SMART_KEYBOARD.bin
copy /b update_txt.hex+SMART_KEYBOARD.bin SMKF_v0100r00.bin /Y


