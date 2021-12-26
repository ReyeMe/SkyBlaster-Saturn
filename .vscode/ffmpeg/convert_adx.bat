@echo off
for /R ".\out" %%f in (*.ADX) do (
    del /Q /F ".\out\%%~nf.ADX" > NUL
)

for /R ".\in" %%f in (*.mp3) do (
    .\ffmpeg -i ".\in\%%~nf.mp3" -f adx -ac 1 -ar 23040 ".\out\%%~nf.ADX"
)

pause