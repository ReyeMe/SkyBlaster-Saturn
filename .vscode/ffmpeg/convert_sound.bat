@echo off
for /R ".\out" %%f in (*.PCM) do (
    del /Q /F ".\out\%%~nf.PCM" > NUL
)

for /R ".\in" %%f in (*.wav) do (
    .\ffmpeg -i ".\in\%%~nf.WAV" -f s8 -ac 1 -ar 15360 ".\out\%%~nf.PCM"
)

pause